#define ADC_PIN 0
#define CD_PIN 8
const float REF_VOLTAGE= 5.00; 
const uint8_t capPins[4]={4, 5, 6, 7};
const float capRes[4]={1000000.0f, 100000.0f, 10000.0f, 1000.0f};
uint8_t counter=0;
bool isDone=false;

float map_it(float val,float in_min,float in_max,float out_min,float out_max){
   float result=(val-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
   return result;
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    for(uint8_t x=0; x<4; x++){
      pinMode(capPins[x], OUTPUT);
    }
    pinMode(CD_PIN, OUTPUT);
    digitalWrite(capPins[0], HIGH);
    //analogReadResolution(10);
}

float avgAdc(){
  float result=0.00;
  for(uint8_t x=0; x<16; x++){
    result+=analogRead(ADC_PIN);
  }
  return result/16.0f;
}

void dischargeCap(){
  while (avgAdc() > 10) {
    digitalWrite(CD_PIN, HIGH);//turn on cap discharge npn for 100millisec
    // keep discharging
  }
  digitalWrite(CD_PIN, LOW);
}

void selectResistor(const uint8_t *index){//helps auto select suitable resistor value
   for (uint8_t i = 0; i < 4; i++) {
      digitalWrite(capPins[i], LOW);
    }
    dischargeCap();
   Serial.println("capacitor discharged...");
   digitalWrite(capPins[*index+1], HIGH);
}

String getUnit(const float C){//helps get capactiro unit
    if (C < 1e-9f) {
        Serial.print(C * 1e12f);
        Serial.println(" pF");
        return String(C * 1e12f)+" pF";
    }
    else if (C < 1e-6f) {
        Serial.print(C * 1e9f);
        Serial.println(" nF");
        return String(C * 1e9f)+" nF";
    }
    else if (C < 1e-3f) {
        Serial.print((C * 1e6f)*1.67f);
        Serial.println(" uF");
        return String(C * 1e6f)+" uF";
    }
}

String getCap(){
   float data= avgAdc();
   if(data>700)isDone=false;//isDone turns true whenever we are done measuring so we don't have to repeat operation before capacitor charges above 700, once above 700 it can be true again since data< 700 won't be true
   if(data<700 && isDone==false){//972 because npn drops about .2v at saturation
      //Serial.println("Operation started....");
      //selectResistor(&counter);//discharge any charge on cap
      long initTime=millis();
      while(data<645.0f){
        data=avgAdc();
        if((millis()-initTime)>1000){
          Serial.println("1 sec elapased, loop broken");
          Serial.print("data:");
          Serial.println(data);
          break;
        }
      }

      if(data<=645.0f && counter<3){//this means if adc value is less than 63% of 1023 after 1 sec then we need to discharge and move to another resistor
        //selectResistor(&counter);
        selectResistor(&counter);
        counter+=1;
      }
      else if(data<=645.0f && counter==4){
          return "Capacitor value out of design range ....";
      }
      else if(data>=643.0f){
          
          unsigned long finalTime=millis();
          unsigned long time=finalTime-initTime;
          Serial.print("Measurement duration:");
          Serial.println(time);
          Serial.println("Counter:"+String(counter));
          float result= ((time/1e3f)/capRes[counter]);
          String capacitance=getUnit(result);
          digitalWrite(capPins[counter], LOW);
          counter=0;
          digitalWrite(capPins[counter], HIGH);//1m ohm bjt stays on for future measurement activelness
          isDone=true;
          //Serial.print("Capacitance:");
          //Serial.println(result);
          return capacitance;
      }
   }
}

void loop() {
  getCap();
  // put your main code here, to run repeatedly:
}
