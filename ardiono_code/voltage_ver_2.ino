void power_mea() {
  unsigned int crossCount = 0;
  unsigned long int numberOfSamples = 0;
  float startV, timeout = 50, lastFilteredV, filteredV = 0, filteredI = 0, sampleV, sampleI, sqV, sumV = 0, sqI, sumI = 0, power, sumP = 0;
  float power_factor, phaseShiftedV;
  boolean lastVCross, checkVCross;
  int crossings = 2, PHASECAL = 1.5;

  // 1) Waits for the waveform to be close to 'zero' (mid-scale adc) part in sin curve.

  unsigned long start1 = millis();
  while (1)
  {
    startV = analogRead(voltage_sensor_pin_num);
    if ((startV < (1023 * 0.55)) && (startV > (1023 * 0.45))) break; //check its within range
    if ((millis() - start1) > timeout) break;
  }

  // 2) Main measurement loop

  start1 = millis();

  while ((crossCount < crossings) && ((millis() - start1) < timeout))
  {
    numberOfSamples++;                       //Count number of times looped.
    lastFilteredV = filteredV;               //Used for delay/phase compensation

    //-----------------------------------------------------------------------------
    // A) Read in raw voltage and current samples
    //-----------------------------------------------------------------------------
    sampleV = analogRead(voltage_sensor_pin_num);                 //Read in raw voltage signal
    sampleI = analogRead(current_sensor_pin_num);                 //Read in raw current signal

    filteredV = (sampleV * 5.0 / 1023.0) - voltage_offset;
    filteredV = (filteredV * tune_parameter) * (223.00 / 14.00);

    //filteredI = (((5.0 * sampleI) / 1023.0) - 2.5) / 0.066;
    filteredI = ((((5.0 * sampleI) / 1023.0) - voltage_offset) * 2000.0) / current_resistance;
    // C) Root-mean-square method voltage

    sqV = filteredV * filteredV;                //1) square voltage values
    sumV += sqV;                                //2) sum

    sqI = filteredI * filteredI;                //1) square current values
    sumI += sqI;                                //2) sum

    phaseShiftedV = lastFilteredV + PHASECAL * (filteredV - lastFilteredV);
    power = phaseShiftedV * filteredI;
    //accumulating sum of the voltage
    sumP += power;

    lastVCross = checkVCross;
    if (sampleV > startV) checkVCross = true;
    else checkVCross = false;
    if (numberOfSamples == 1) lastVCross = checkVCross;

    if (lastVCross != checkVCross) crossCount++;
  }
  filteredV = sqrt(sumV / numberOfSamples);
  filteredI = sqrt(sumI / numberOfSamples);
  power = sumP / numberOfSamples;
  if(power<0.1){
    power = 0.1;
  }
  //  power_factor = power /(filteredV*filteredI);
  //  Serial.print(filteredV);
  //  Serial.print(" V ");
  //  Serial.print(filteredI);
  //  Serial.print(" A ");
  //  Serial.print(power);
  //  Serial.print(" W ");
  //  Serial.println(" ");

  if (!digitalReadOutputPin(local_relay)) {
    if (filteredI > max_current_limit) {
      unsigned long overcurrent_time = millis();
      while (1) {
        if ((millis() - overcurrent_time) > over_current_stay) {
          digitalWrite(local_relay, HIGH);

          display.clearDisplay(); // Clear the Screen
          display.setTextSize(2); // Set text Size
          display.setTextColor(WHITE); // set LCD Colour
          display.setCursor(30, 0); // Set Cursor Position
          display.print("Over_current_detected");
          display.display();

          digitalWrite(red_led, HIGH);
          digitalWrite(buzzer, HIGH);
          overcurrent_voltage = true;

        }
        if (current_mea2() < max_current_limit) {
          break;
        }
      }
    }
  }

  if (!digitalReadOutputPin(local_relay)) {
    if (filteredV > max_voltage_limit) {
      digitalWrite(local_relay, HIGH);

      display.clearDisplay(); // Clear the Screen
      display.setTextSize(2); // Set text Size
      display.setTextColor(WHITE); // set LCD Colour
      display.setCursor(30, 0); // Set Cursor Position
      display.print("Over_Voltage_detected");
      display.display();

      digitalWrite(red_led, HIGH);
      digitalWrite(buzzer, HIGH);
      overcurrent_voltage = true;
    }

    if (filteredV < min_voltage_limit) {
      digitalWrite(local_relay, HIGH);

      display.clearDisplay(); // Clear the Screen
      display.setTextSize(2); // Set text Size
      display.setTextColor(WHITE); // set LCD Colour
      display.setCursor(30, 0); // Set Cursor Position
      display.print("Lower_Voltage_detected");
      display.display();

      digitalWrite(red_led, HIGH);
      digitalWrite(buzzer, HIGH);
      overcurrent_voltage = true;
    }
  }

  if (!overcurrent_voltage) {
    display.clearDisplay(); // Clear the Screen
    display.setTextSize(2); // Set text Size
    display.setTextColor(WHITE); // set LCD Colour
    display.setCursor(30, 0); // Set Cursor Position
    display.print(filteredV);
    display.print("V ");// Print the this Text
    display.print(filteredI);
    display.print("A ");
    display.print(power);
    display.print(" W");
    display.display(); // Update the Display
  }
  //  Serial.print(" Power factor: ");
  //  Serial.println(power_factor);

}
