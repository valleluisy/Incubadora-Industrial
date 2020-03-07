/* ---------------------------CODIGO INCUBADORA LV-40------------------------* 
 * Codigo de control del prototipo funcional de incubadora LV-40 presentado  
 * por Luis Valle como proyecto de graduación, consultas o sugerencias en el  
 * diseño podran ser recibidas mediante correo lyanes21@yahoo.es 
 * 
 * Version 4.4 Actualizada el 8/05/2017 por Luis Valle  
 */
 
#include <TimeLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include "cactus_io_BME280_I2C.h" 
LiquidCrystal_I2C lcd(0x27,20,4);
time_t Tiempo=0;
BME280_I2C bme(0x76);

//------------------------------------VARIABLES DE ENTRADA--------------------------//
int BotonIncubar=5;
int BotonSi=6;
int BotonNo=7;
int BotonDetener=8;

//-----------------------------------VARIABLES DE SALIDA-----------------------------//
int Buzzer=9;
int PinVentilacion=10;
int PinCalefaccion=12;
int PinRotacion=11;

//-----------------------------------VARIABLES DE ESTADOS Y CONTADORES--------------------------//
int Dia=0;
int TiempoRotacion=0;
int Condicion=0;
int CondicionCalibrarBandeja=0;
int ContadorRotacion=3;
int Contador2=0;

int EstadoCalefaccion=0;
int EstadoVentilacion=0;
int EstadoRotacion=0;

float Temperatura=0;
float Humedad=0;

//-----------------------------------------------------CONFIGURACION INICIAL------------------------------------------------------------//
void setup()
{
  //bme.setTempCal(-1);
  pinMode(Buzzer, OUTPUT);
  pinMode(PinRotacion, OUTPUT);
  pinMode(BotonIncubar, INPUT);
  pinMode(BotonSi, INPUT);
  pinMode(BotonNo, INPUT);
  pinMode(BotonDetener, INPUT);
  Buzzer_inicio(); 
  lcd.init();
  lcd.backlight();
  bme.begin();
}

//------------------------------------------------------------PROGRAMA PRINCIPAL-----------------------------------------------------------------//
void loop()
{
  PantallaIntroduccion();
  Dia=0;
  delay(1000);
  
  if(digitalRead(BotonIncubar)==1)
  { 
    RotacionIF();
    Buzzer_sonido1();
    InicioDataExcel();
    setTime(00,00,00,1,1,2017);
    while(Dia< 21)
    { 
      bme.readSensor();
      Temperatura=bme.getTemperature_C();
      Humedad=bme.getHumidity();
      Tiempo=now();
      Dia=day(Tiempo);
      PantallaIncubacion();
      Estado_Ventilacion();
      Estado_Rotacion();
      Estado_Calefaccion();
      DataExcel();
      if(digitalRead(BotonSi)==1)
      {
       OpcionesCalibracionBandeja();
      }
      CondicionDetenerIncubacion();
            
      delay(1000);
    }
  }
  
  if(digitalRead(BotonDetener)==1)
  {
    MenuOpciones();
  }

}

//-----------------------------------------------------------PANTALLAS Y SUB-MENUS------------------------------------------------------------------//
void PantallaIntroduccion()
{
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("INCUBADORA LV-40");
  lcd.setCursor(0,1);
  lcd.print("prototipo funcional");
  lcd.setCursor(0,3);
  lcd.print("esperando.");
  delay(500);
  lcd.setCursor(0,3);
  lcd.print("esperando..");
  delay(500);
  lcd.setCursor(0,3);
  lcd.print("esperando...");
}
void PantallaIncubacion()
{
  lcd.clear();
  lcd.print("INCUBANDO");
  lcd.setCursor(0, 1);    
  lcd.print("Temperatura=");
  lcd.print(Temperatura);
  lcd.setCursor(0, 2);     
  lcd.print("Humedad="); 
  lcd.print(Humedad);
  lcd.setCursor(0, 3);    
  lcd.print("Dia="); 
  lcd.print(day(Tiempo));
  lcd.setCursor(7, 3);
  lcd.print(" ");
  lcd.setCursor(8, 3);    
  lcd.print(hour(Tiempo));
  lcd.setCursor(10, 3);
  lcd.print(":");
  lcd.setCursor(11, 3);   
  lcd.print(minute(Tiempo)); 
  lcd.setCursor(17, 3);   
  lcd.print(ContadorRotacion); 
}

void PantallaDetenerIncubacion()
{
  lcd.clear();
  lcd.print("Esta seguro?");
  lcd.setCursor(18, 1);
  lcd.print("SI");
  lcd.setCursor(18, 2);
  lcd.print("NO");
}
void PantallaMenuOpciones()
{
  lcd.clear();
  lcd.setCursor(8,0);
  lcd.print("DEMOSTRACION");
  lcd.setCursor(4,1);
  lcd.print("CALIBRAR BANDEJA");
  lcd.setCursor(15,2);
  lcd.print("SALIR");
}

void MenuOpciones()
{ 
  Buzzer_sonido1();
  Condicion=0;
  while(Condicion<1)
  {
   PantallaMenuOpciones(); 
   delay(1000);

    if(digitalRead(BotonIncubar)==1)
      {
        OpcionesDemostracion();
      }
    if(digitalRead(BotonSi)==1)
      {
       OpcionesCalibracionBandeja();
      }
    if(digitalRead(BotonNo)==1)
      {
       Buzzer_sonido2();
       Condicion=1;
      }      
  }    
}
//--------------------------------------------------------CONDICIONES DE CONTROL--------------------------------------------------------//

void CondicionDetenerIncubacion()
{
 if(digitalRead(BotonDetener)==1)
   {
    Buzzer_sonido2();
    PantallaDetenerIncubacion();
    Condicion=0;
    while(Condicion<1)
     {
      if(digitalRead(BotonSi)==1)
        {
          Condicion=1;
          Dia=21;
          digitalWrite(PinCalefaccion, LOW);
          digitalWrite(PinVentilacion, LOW);
          digitalWrite(PinRotacion, LOW);
          Serial.end();
          ContadorRotacion=3;
          Contador2=0;
          digitalWrite(PinRotacion, HIGH);
          delay(2000);
          digitalWrite(PinRotacion, LOW);
          delay(10); 
        }
      if(digitalRead(BotonNo)==1)
        {
          Buzzer_sonido2();
          Condicion=1;
        }
      delay(200);
     }
   }
}

void OpcionesDemostracion()
{
  Buzzer_sonido0();
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Modo Demostracion");
  lcd.setCursor(0,1);
  lcd.print("T°C   H%"); 
  lcd.setCursor(15,3);
  lcd.print("SALIR");
  Condicion=0;         
  while(Condicion<1)
    { 
      bme.readSensor();
      Temperatura=bme.getTemperature_C();
      Humedad=bme.getHumidity();
      lcd.setCursor(0,2);
      lcd.print(Temperatura);
      lcd.setCursor(7,2);
      lcd.print(Humedad);
      delay(2000);
     if(digitalRead(BotonDetener)==1)
       {
        Buzzer_sonido2();
        Condicion=1;
       }
    }  
}

void OpcionesCalibracionBandeja()
{ 
  Buzzer_sonido3();  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Modo Calibracion");
  lcd.setCursor(15,2);
  lcd.print("MOVER"); 
  lcd.setCursor(15,3);
  lcd.print("SALIR");
  CondicionCalibrarBandeja=0;       
  while(CondicionCalibrarBandeja<1)
    {

     if(digitalRead(BotonNo)==1)
       {
        digitalWrite(PinRotacion, HIGH);
        delay(200);
       }
     digitalWrite( PinRotacion, LOW);     
     if(digitalRead(BotonDetener)==1)
       {
        //Buzzer_sonido2();
        CondicionCalibrarBandeja=1;
       }
    }                
}

void Estado_Rotacion()
{ TiempoRotacion=ContadorRotacion- hour(Tiempo);

 if( ContadorRotacion == 24 )
   {
     ContadorRotacion=0;
   }  
 
 if( TiempoRotacion == 0 && day(Tiempo)<19 )
   {
    EstadoRotacion=1;
    digitalWrite(PinRotacion, HIGH);
    ContadorRotacion=ContadorRotacion+3;
    delay(3900);
   } 
 else
   {
    EstadoRotacion=0;
    digitalWrite(PinRotacion, LOW);
    delay(100);
   }  
 if( day(Tiempo)>=19 && Contador2==0 )
   {
    Contador2=1;
    digitalWrite(PinRotacion, HIGH);
    delay(2100);
    digitalWrite(PinRotacion, LOW);
    delay(50);  
   }   
}
void RotacionIF()
{ 
    digitalWrite(PinRotacion, HIGH);
    delay(2100);
    digitalWrite(PinRotacion, LOW);
    delay(50);  
}

void Estado_Ventilacion()
{
 if(Temperatura <= 37.0 )
   {
    EstadoVentilacion=1;
    analogWrite(PinVentilacion, 255);
    delay(50);
   } 
 if(Temperatura >= 38.0 )
   {
    EstadoVentilacion=0;
    analogWrite(PinVentilacion, 150);
    delay(50);
   }     
}

void Estado_Calefaccion()
{
 if(Temperatura <= 37.0 )
   {
    EstadoCalefaccion=1;
    digitalWrite(PinCalefaccion, HIGH);
    delay(50);
   }
 if(Temperatura >= 38.0 )
   {
    EstadoCalefaccion=0;
    digitalWrite(PinCalefaccion, LOW);
    delay(50);
   }   
}

//-----------------------------------------------------------SONIDOS Y ALERTAS----------------------------------------------------------//
void Buzzer_inicio()
{
  tone(Buzzer,293,100);
  delay(100);
  tone(Buzzer,440,100);
  delay(100);
  tone(Buzzer,523,100);
  delay(100);
  tone(Buzzer,587,100);
  delay(200);
  tone(Buzzer,587,100);
  tone(Buzzer,698,100);
  return;    
}
void Buzzer_sonido0()
{
  tone(9,440,100);
  delay(100);
  tone(9,520,100);
  delay(100);
  tone(9,550,100);
  delay(200);
  tone(9,550,100);
  delay(200);
  tone(9,550,100);
  delay(100);
  tone(9,650,100);
  tone(9,700,100);
  return;    
}
void Buzzer_sonido1()
{
  tone(Buzzer,200,100);
  delay(100);
  tone(Buzzer,300,100);  
  return;
}
void Buzzer_sonido2()
{
  tone(Buzzer,100,100);
  delay(100);
  tone(Buzzer,400,100);  
  return;
}
void Buzzer_sonido3()
{
  tone(Buzzer,100,100);
  delay(100);
  tone(Buzzer,800,100);  
  return;
}
//------------------------------------------------------------DATOS A PC-----------------------------------------------------------------------------//
void InicioDataExcel()
{
 Serial.begin(9600);
 Serial.println("CLEARSHEET"); // clears starting at row 1
 Serial.println("LABEL,Date,Time,Dia,Hora,Minuto,Temperatura,Humedad,Ventilacion,Rotacion,Calefaccion");
}

void DataExcel()
{
 if(second(Tiempo)==0 || second(Tiempo)==1|| second(Tiempo)==10|| second(Tiempo)==11|| second(Tiempo)==20|| second(Tiempo)==21|| second(Tiempo)==30|| second(Tiempo)==31|| 
   second(Tiempo)==40|| second(Tiempo)==41|| second(Tiempo)==50|| second(Tiempo)==51)
   {  
    Serial.print("DATA,DATE,TIME,");
    Serial.print(day(Tiempo)); Serial.print(",");
    Serial.print(hour(Tiempo)); Serial.print(",");
    Serial.print(minute(Tiempo)); Serial.print(","); 
    Serial.print(Temperatura); Serial.print(",");
    Serial.print(Humedad); Serial.print(",");
    Serial.print(EstadoVentilacion); Serial.print(",");
    Serial.print(EstadoRotacion); Serial.print(",");
    Serial.println(EstadoCalefaccion);
   }
}



