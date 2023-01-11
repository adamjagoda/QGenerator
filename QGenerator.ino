
#include <Arduino.h>

bool newCmd = false;
String cmdString = ""; 

class QuadratureGeneratorClass
{
  public:
    int m_state;
    int m_lastState;
    unsigned long m_frequency;
    int m_pin1;
    int m_pin2;
    unsigned long m_pulses ;
    unsigned long m_delta ;
    bool m_reverse ;
    void wait();

  public:
    void initialize(int pin1, int pin2, unsigned long frequency, bool invert);
    void generate(unsigned long numOfPulses, unsigned long frequency, bool invert);
    void increment();
    void decrement();
    void fsm();

};

void QuadratureGeneratorClass::initialize(int pin1, int pin2, unsigned long frequency, bool invert)
{
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  m_pin1 = pin1;
  m_pin2 = pin2;
  m_reverse  = invert;
  m_frequency = frequency;
  m_state = -1;
  m_lastState = -2;
}

void QuadratureGeneratorClass::wait()
{
  if(m_frequency >= 10)
    delayMicroseconds(m_delta );
  else  
    delay(m_delta ); 
}

void  QuadratureGeneratorClass::fsm()
{
  switch (m_state) 
  {
    case 0:
      PORTD &= ~(1<<m_pin1) ;
      PORTD &= ~(1<<m_pin2) ;
      if(m_pulses  > 0)
      {
        m_pulses--;
        wait();
        if (m_reverse  == false) 
        {
          m_state = 2;
        }
        else
        {
          m_state = 1;
        } 
       }
      else 
      {
        m_state = -1;
      }
    break;
    case 1:
      PORTD |=  (1<<m_pin1) ;
      PORTD &= ~(1<<m_pin2) ;
      wait();
      if (m_reverse  == false) 
      {
        m_state = 0;
      }
      else
      {
        m_state = 3;
      }
    break;
    case 2:
      PORTD &= ~(1<<m_pin1) ;
      PORTD |=  (1<<m_pin2) ;
      wait();
      if (m_reverse  == false) 
      {
        m_state = 3;
      }
      else
      {
        m_state = 0;
      }
    break;
    case 3:
      PORTD |= (1<<m_pin1) ;
      PORTD |= (1<<m_pin2) ;
      wait();
      if (m_reverse == false) 
      {
        m_state = 1;
      }
      else
      {
        m_state = 2;
      }
    break;
    default:
    break;
  }
  m_lastState = m_state;
}

void QuadratureGeneratorClass::generate(unsigned long numOfPulses,unsigned long frequency,bool invert)
{
    
  if(numOfPulses > 0)
  {
    m_pulses  = numOfPulses;
    m_reverse  = invert;
    m_frequency = frequency;
    if(m_frequency >= 10)
    {
      //in us
      m_delta  = (unsigned long)((1000000.0 / (4.0*(double)m_frequency)));
    }
    else 
    {
      //in ms
      m_delta  = (unsigned long)((1000.0 / (4.0*(double)m_frequency)));
    }
    m_state = 0;
  }
  else
  {
    m_state = -1;
  }
  
}

void QuadratureGeneratorClass::increment()
{
  generate(m_pulses ,m_frequency,false);
}

void QuadratureGeneratorClass::decrement()
{
  generate(m_pulses ,m_frequency,true);
}

QuadratureGeneratorClass generator;

String CommandCom(const char * dataIn, size_t lenIn)
{
   String inputString = "\0";
   String outputString = "CommandCom >";

   if (dataIn == NULL)
   {
     return outputString;
   }

   for(int i=0; i < lenIn; i++)
   {
     inputString += char(dataIn[i]);
   }

   outputString += inputString + "\n";

   int separatorIndex = inputString.indexOf(":");
   signed long int ntValue = 0;
   String variableName = inputString.substring(0,separatorIndex);
   
   String itsValue = (separatorIndex >=0) ? inputString.substring(separatorIndex+1) : "\0";

   variableName.trim();
   variableName.toLowerCase(); 

   if(variableName == "help")
   {
      outputString += "supported commands:\nhelp\n";   
   }
   else if ( variableName == "initialize") 
   {
     outputString += itsValue;
     outputString += "\nDONE!\n";  
     generator.initialize(2,3,50,false);
     
   }
   else if ( variableName == "generate") 
   {
     outputString += itsValue;
     outputString += "\nDONE!\n";  
     
     intValue = 1;
     if(itsValue!="\0")
     {
      intValue = itsValue.toInt(); 
     }
     //generator.generate(intValue,generator.m_frequency,generator.m_reverse );

   }
   else if ( variableName == "increment") 
   {
     outputString += itsValue;
     outputString += "\nDONE!\n";  
     
     intValue = 1;
     if(itsValue!="\0")
     {
       intValue = itsValue.toInt(); 
     }
     
     if(intValue < 0 )
     {
       intValue = 1;
     }

     if(intValue > 1000000 )
     {
       intValue = 1000000;
     }

     generator.m_pulses  = intValue;
     generator.increment();
   }
   else if ( variableName == "decrement") 
   {
     outputString += itsValue;
     outputString += "\nDONE!\n";  
     
     intValue = 1;
     if(itsValue!="\0")
     {
       intValue = itsValue.toInt(); 
     }
     
     if(intValue < 0 )
     {
       intValue = 1;
     }

     if(intValue > 1000000 )
     {
       intValue = 1000000;
     }

     
     generator.m_pulses  = intValue;
     generator.decrement();
   }
   else if ( variableName == "frequency") 
   {
     outputString += itsValue;
     outputString += "\nDONE!\n";  
     
     intValue = 1;
     if(itsValue!="\0")
     {
       intValue = itsValue.toInt(); 
     }
     
     if(intValue < 0 )
     {
       intValue = 1;
     }

     if(intValue > 1000000 )
     {
       intValue = 1000000;
     }

     generator.m_frequency = intValue;
     
   }
   else if ( variableName == "dump") 
   {
     outputString += itsValue;
     
     outputString += "pulses:";
     outputString += String(generator.m_pulses );
     outputString += "\n";

     outputString += "frequency:";
     outputString += String(generator.m_frequency);
     outputString += "\n";

     outputString += "period:";
     outputString += String(1.0 / (double)generator.m_frequency);
     outputString += "\n";

     outputString += "invert:";
     outputString += String(generator.m_reverse );
     outputString += "\n";

     outputString += "tq";
     if(generator.m_frequency>=10)
     {
       outputString += "[us]:";
      generator.m_delta  = (unsigned long)((1000000.0 / (4.0*(double)generator.m_frequency)));
     } 
     else
     {
       outputString += "[ms]:";
      generator.m_delta  = (unsigned long)((1000.0 / (4.0*(double)generator.m_frequency)));
     } 
     outputString += String(generator.m_delta );
     outputString += "\n";

     outputString += "pin1:";
     outputString += String(generator.m_pin1);
     outputString += "\n";

     outputString += "pin2:";
     outputString += String(generator.m_pin2);
     outputString += "\n";

     outputString += "\nDONE!\n";  
   }
   else 
   {
      outputString += "not supported command \n";  
   }
   return outputString; 
}   

void serialListener()
{
  if(Serial.available()) 
  {
    char inChar = (char)Serial.read();
    if ((inChar == '\r') || (inChar == '\n') || (inChar == '\0')) 
    {
      newCmd = true;
    }
    cmdString += inChar;
   }

  if(newCmd)
  {
    Serial.println(CommandCom((const char *)cmdString.c_str(), (size_t)cmdString.length()));
    cmdString = "";
    newCmd = false;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  cmdString.reserve(128);
  //cmdString.clear();
  cmdString = "";
  newCmd = false;
  while(!Serial){;}
  generator.initialize(2, 3, 50, false);
}

void loop() {
  // put your main code here, to run repeatedly:
  generator.fsm();
  serialListener();
}
