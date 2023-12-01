/*!
\file   maquinaestados.ino
\date   2023-30-11
\author Andres Castro <andresfcastro@unicauca.edu.co> - Daniel Tucanes <danieltucanes@unicauca.edu.co> - Sebastian Pisso <jpisso@unicauca.edu.co>
\brief  Smart greenhouse Program.

\par Copyright
Information contained here in is proprietary to and constitutes valuable
confidential trade secrets of Unicauca, and
is subject to restrictions on use and disclosure.

\par
Copyright (c) Unicauca 2022. All rights reserved.

\par
The copyright notices above do not evidence any actual or
intended publication of this material.
******************************************************************************
*/


/**
* @name setup_State_Machine
*----------------------------------------------------------------------------
* @param None.
* @return currentInput.
*----------------------------------------------------------------------------
* @brief:
* Setup the State Machine
*----------------------------------------------------------------------------
*
*/
void setup_State_Machine()
{
  // Add transitions
  stateMachine.AddTransition(INICIAL, BLOQUEADO, []() { return currentInput == AVANZARN; });
  stateMachine.AddTransition(BLOQUEADO, INICIAL, []() { return currentInput == RETROCEDER; });
  stateMachine.AddTransition(INICIAL, MONITORTyH, []() { return currentInput == AVANZARP; });
  
  stateMachine.AddTransition(MONITORTyH, MONITORLUZ, []() { return currentInput == AVANZARN; });
  stateMachine.AddTransition(MONITORLUZ, MONITORTyH, []() { return currentInput == RETROCEDER; });
  stateMachine.AddTransition(MONITORTyH, ALARMA, []() { return currentInput == AVANZARP; });
  stateMachine.AddTransition(ALARMA, MONITORTyH, []() { return currentInput == RETROCEDERN; });
  
  stateMachine.AddTransition(MONITORLUZ, ALARMA, []() { return currentInput == AVANZARP; });


  stateMachine.SetOnEntering(INICIAL, intput_inicial);
  stateMachine.SetOnEntering(BLOQUEADO, input_bloqueado);
  stateMachine.SetOnEntering(MONITORTyH, input_monitortyh);
  stateMachine.SetOnEntering(MONITORLUZ, input_monitorluz);
  stateMachine.SetOnEntering(ALARMA, input_compalarma);

  stateMachine.SetOnLeaving(INICIAL, output_inicial);
  stateMachine.SetOnLeaving(BLOQUEADO, output_bloqueado);
  stateMachine.SetOnLeaving(MONITORTyH, output_monitortyh);
  stateMachine.SetOnLeaving(MONITORLUZ, output_monitorluz);
  stateMachine.SetOnLeaving(ALARMA, output_compalarma);

}

/**
* @name intput_inicial
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when entering state INICIAL
*----------------------------------------------------------------------------
*
*/
void intput_inicial(){
  //Put a transition to avoid changes to other states
  currentInput = Input::DESCONOCIDO;
  //Starting Asynchronous Task
  contrasenia.Start();
}

/**
* @name output_inicial
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when leaving state INICIAL
*----------------------------------------------------------------------------
*
*/
void output_inicial(){
  //Stopping Asynchronous Task
  contrasenia.Stop();
}

/**
* @name writepassword
*----------------------------------------------------------------------------
* @param None.
* @return result.
*----------------------------------------------------------------------------
* @brief:
* Recieve the password with condition about between numbers
*----------------------------------------------------------------------------
*
*/
String writepassword(){
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);
  lcd.print("Write the");
  lcd.setCursor(0, 1);
  lcd.print("password: ");
  String result = "";
  while (result.length() < 4) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      lcd.print("*");
      result += key;
    }
  }
  lcd.clear();
  return result;
}

/**
* @name trypassword
*----------------------------------------------------------------------------
* @param None.
* @return  Nothing.
*----------------------------------------------------------------------------
* @brief
* Calling function writepassword check if password is correct or not
*----------------------------------------------------------------------------
* 
*/
void trypassword(){
  //Make a counter
  int c = 0;
  //Verify if is less than 3(Verify the three times that the user can put the password)
  while(c < 3){
    //Verify if the password is equal
    if(writepassword().equals(password)){
      digitalWrite(R, LOW);
      digitalWrite(G, HIGH);
      digitalWrite(B, LOW);
      lcd.print("Correct password");
      delay(1000);
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
      currentInput = Input::AVANZARP;
      //Serial.println("Entra");
      c = 5;

    }else{
      //Verify if counter is more than three (Access denied for the user if more than three else let the user do other try)
      if(c >= 2){
        digitalWrite(R, HIGH);
        digitalWrite(G, LOW);
        digitalWrite(B, LOW);
        lcd.print("Sorry, u can not");
        lcd.setCursor(0, 1);
        lcd.print("join :(");
        currentInput = Input::AVANZARN;
        lcd.print(currentInput);
        c=5;
      }
      else{
        digitalWrite(R, LOW);
        digitalWrite(G, LOW);
        digitalWrite(B, HIGH);
        lcd.print("Wrong password");
        delay(500);
        lcd.setCursor(0, 1);
        lcd.print("Try again");
        delay(1000);
        digitalWrite(R, LOW);
        digitalWrite(G, LOW);
        digitalWrite(B, LOW);
        lcd.clear();
        c++;
      }
    }
  }
}

/**
* @name input_bloqueado
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when entering state BLOQUEADO
*----------------------------------------------------------------------------
*
*/
void input_bloqueado(){
  //Serial.println("Entra a bloqueado");

  //Put a transition to avoid changes to other states
  currentInput = Input::DESCONOCIDO;
  //Starting Asynchronous Task
  tm5.Start();
}

/**
* @name output_bloqueado
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when leaving state BLOQUEADO
*----------------------------------------------------------------------------
*
*/
void output_bloqueado(){
  //Stopping Asynchronous Task
  tm5.Stop();
}

/**
* @name taketemperature
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Read and show the temperature and humidity values and compares to make trasitions
*----------------------------------------------------------------------------
*
*/
void taketemperature(){
  lcd.clear();
  lcd.setCursor(0,0);
  dhtObject.begin();
  //Read values of temperature and humidity
  int humi = dhtObject.readHumidity();
  int temp = dhtObject.readTemperature();
  //Show the values readed
  //Serial.println("Humedad: ");
  lcd.print("H: ");
  //Serial.println(humi);
  lcd.print(humi);
  lcd.print(" ");
  // Add a new random value to the bucket
  ave1.push(humi);
  lcd.print("P.H: ");
  lcd.print(ave1.mean());
  lcd.setCursor(0,1);
  //Serial.println("Temperatura: ");
  lcd.print("T: ");
  //Serial.println(temp);
  lcd.print(temp);
  ave2.push(temp);
  lcd.print("P.T: ");
  lcd.print(ave2.mean());

  delay(1000);
  //Compare the values
  if(temp > 28 || ave1.mean() > 70){
    currentInput = Input::AVANZARP;
    //Serial.println("Entra");
  }
  //Serial.println(currentInput);
}


/**
* @name input_monitortyh
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when entering state MONITORTyH
*----------------------------------------------------------------------------
*
*/
void input_monitortyh(){
  //Serial.println("Entra a monitor de humedad y temperatura");

  //Put a transition to avoid changes to other states
  currentInput = Input::DESCONOCIDO;

  //Starting Asynchronous Task
  temperatura.Start();
  tm52.Start();
}

/**
* @name output_monitortyh
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when leaving state MONITORTyH
*----------------------------------------------------------------------------
*
*/
void output_monitortyh(){
  //Stopping Asynchronous Task
  temperatura.Stop();
  tm52.Stop();
}

/**
* @name takelight
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Read and show photoresistor value and compares to make trasitions
*----------------------------------------------------------------------------
*
*/
void takelight(){
  lcd.clear();
  lcd.setCursor(0,0);
  //Read the value
  int analogValue = analogRead(LIGHTPIN);
  lcd.print("Valor luz: ");
  //Show the value
  lcd.print(analogValue);
  ave3.push(analogValue);
  lcd.setCursor(0,1);
  lcd.print("P. luz: ");
  lcd.print(ave3.mean());
  delay(1000);
  //Compare the value
  if(ave3.mean() < 20){
    currentInput = Input::AVANZARP;
  }
}

/**
* @name input_monitorluz
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when entering state MONITORLUZ
*----------------------------------------------------------------------------
*
*/
void input_monitorluz(){
  //Serial.println("Entra a monitor de luz");

  //Put a transition to avoid changes to other states
  currentInput = Input::DESCONOCIDO;

  //Starting Asynchronous Task
  luz.Start();
  tm3.Start();
}

/**
* @name output_monitorluz
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when leaving state MONITORLUZ
*----------------------------------------------------------------------------
*
*/
void output_monitorluz(){
  //Stopping Asynchronous Task
  luz.Stop();
  tm3.Stop();
}

/**
* @name turnalarm
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute the alarm song
*----------------------------------------------------------------------------
*
*/
void turnalarm(){
  //ave3.clear();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alarma");
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
}

/**
* @name input_compalarma
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when entering state ALARMA
*----------------------------------------------------------------------------
*
*/
void input_compalarma(){
  //Serial.println("Entra a alarma");

  //Put a transition to avoid changes to other states
  currentInput = Input::DESCONOCIDO;

  //Starting Asynchronous Task
  alarm.Start();
  tm6.Start();
}

/**
* @name output_compalarma
*----------------------------------------------------------------------------
* @param None.
* @return Nothing.
*----------------------------------------------------------------------------
* @brief:
* Execute actions when leaving state ALARMA
*----------------------------------------------------------------------------
*
*/
void output_compalarma(){
  //Stopping Asynchronous Task
  alarm.Stop();
  tm6.Stop();
}
