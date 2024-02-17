void vol_mea_2() {
  unsigned int crossCount = 0;                             
  unsigned long int numberOfSamples = 0;                        
  float startV, timeout = 50, lastFilteredV, filteredV = 0, filteredI = 0, sampleV, sampleI, sqV, sumV = 0, sqI, sumI = 0, power, sumP = 0;
  boolean lastVCross, checkVCross;
  int crossings = 2;

  // 1) Waits for the waveform to be close to 'zero' (mid-scale adc) part in sin curve.
  
  unsigned long start1 = millis();    
  while (1)                                  
  {
    startV = analogRead(A1);                    
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
    sampleV = analogRead(A1);                 //Read in raw voltage signal

    // B) Apply digital low pass filters 
    filteredV = (sampleV * 5.0 / 1023.0) - voltage_offset;
    filteredV = (filteredV * tune_parameter) * (223.00/14.00);

    // C) Root-mean-square method voltage
    
    sqV = filteredV * filteredV;                //1) square voltage values
    sumV += sqV;                                //2) sum

    lastVCross = checkVCross;
    if (sampleV > startV) checkVCross = true;
    else checkVCross = false;
    if (numberOfSamples == 1) lastVCross = checkVCross;

    if (lastVCross != checkVCross) crossCount++;
  }
  filteredV = sqrt(sumV / numberOfSamples);
  Serial.print(filteredV);
  Serial.println(" V ac");
  
  /*display.clearDisplay(); // Clear the Screen
  display.setTextSize(2); // Set text Size
  display.setTextColor(WHITE); // set LCD Colour
  display.setCursor(30, 0); // Set Cursor Position
  display.print(filteredV);
  display.println("V");// Print the this Text
  display.display(); // Update the Display
  */
}
