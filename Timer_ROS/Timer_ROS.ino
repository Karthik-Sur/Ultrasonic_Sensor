/* 
   *This program is used to find the distance of objects(obstacles) from the sensor.
   
   *This program uses the NewPing headerfile to find the distance of objects in "cm". The value is returned by ping_cm() in cm.
   
   *Karthik is the authour
   
*/
#include<ros.h>                                               //ros/ros.h is a convenience include that includes all the headers necessary to use the most common public pieces of the ROS system.
#include<std_msgs/Int16.h>                                    //This includes the std_msgs/Int16 message, which resides in the std_msgs package. This headerfile would facilitate ROS to send/receive data of integer type.
ros::NodeHandle nh;                                           //Instantiate the node handle, which allows our program to create publishers and subscribers.
std_msgs::Int16 centi;                                        //Declaring a class named centi.
ros::Publisher chatter("chatter", &centi);                    //Here we instantiate a Publisher with a topic name of "chatter". The second parameter to Publisher is a reference to the message instance to be used for publishing.
#include<NewPing.h>                                           //Included NewPing Function which is meant exclusively for Ultrasoinc Sensor. You can download NewPing library from "http://forum.arduino.cc/index.php?topic=106043.0".
#define MAX_DISTANCE 300                                      
NewPing sonar1(13,13,MAX_DISTANCE);                           //A function sonar1 declared in Class NewPing;sonar1(trigger pin,Echo pin,Maximum Distance you want the Sensor to measure). It is to note that one of the advantages of using the NewPing
NewPing sonar2(12,12,MAX_DISTANCE);                           //headerfile is that you can have the same pin for both Trigger and Echo signals. Here, I have used pins 11,12 and 13 of Arduino to integrate with the sensor.
NewPing sonar3(11,11,MAX_DISTANCE);
unsigned int m_cm;
unsigned int getMeasurement(char x);
boolean a=false,b=false,c=false;
void setup() {
        nh.initNode();                                        //In the Arduino setup function you need to initialize your ROS node handle, advertise any 
        nh.advertise(chatter);                                //topics being published, and subscribe to any topics you wish to listen to.
        TCCR1A=0;                                             //Set register A to 0.
        TCCR1B=0;                                             //Set register B to 0.
        TCNT1=0;                                              //Set counter value to 0.
        TIFR1=0;                                              //Set Timer/Counter1 Interrupt Flag Register to 0.
        TCCR1B |= (1<<CS11) | (1<<CS10);                      //Set prescaler to 64.
        TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B) | (1<<OCIE1C);    //Timer/Counter1 Output Compare A/B/C Match interrupt is enabled.
        OCR1C=7499;                                           //30 mS delay: set frequency to (100/3)Hz;Make sure that the value is below 65,536 for Timer 1,which is 16 bit.
        OCR1B=14999;                                          //30 mS delay after Sensor1: set frequeny to (50/3)Hz;A 30mS delay is delibrately given so that the Sensors Signal do not interfere with each other.
        OCR1A=22499;                                          //30 mS delay after Sensor2: set frequency to (100/9)Hz; the OCR1A Register is used to manipulate the counter resolution.
                                                              //The counter value (TCNT1) increases until a compare match occurs with OCR1A, and then counter (TCNT1) is cleared.
        interrupts();                                         //Enable Interrupts.
} 

//compare match register TCNT1= [(16*(10^6))/(prescaler(64 in this case)*desired interrupt frequency)]-1;   where 16*(10^6)-Frequency at which the Microcontroller works.
//It should be noted that Serial command does not work when you use rosserial. Eg: Serial.print(), Serial.begin() would give you an error.
void loop() {
        if(c) {                                               //Activate Sensor3
            m_cm=getMeasurement(3);                           //Get the distance of Obstacle in cm for sensor 3
            centi.data=m_cm;                                  //Copy the value to "data" of int_16 datatype defined in class centi.
            chatter.publish( &centi);                         //Now we actually broadcast the message to anyone who is connected.
            nh.spinOnce();                                    //This command is used to receive any callbacks if there was a subscription into this application.
            c=false;                                          //Stop executing the commands in this if statement.
        } 
        if(b) {                      
            m_cm=getMeasurement(2);                           //Activate Sensor2
            centi.data=m_cm;                                  //Same Logic as Sensor3
            chatter.publish( &centi);
            nh.spinOnce();
            b=false;
        }
        if(a) {             
            m_cm=getMeasurement(1);                           //Activate Sensor1 
            centi.data=m_cm;
            chatter.publish(&centi);
            nh.spinOnce();
            a=false;
        }
 
}
ISR(TIMER1_COMPC_vect) {                                      //Interrupt Service Routine; The ISR(TIMER1_COMPC_vect) is called when TCNT1=OCR1C(7499 in this case).
        c=true;                                               //Execute the corresponding code in void loop(). 
}
ISR(TIMER1_COMPB_vect) {                                      //The ISR(TIMER1_COMPB_vect) is called when TCNT1=OCR1B(14999 in this case).
        b=true;                                               //Execute the corresponding code in void loop().
}
ISR(TIMER1_COMPA_vect) {                                      //The ISR(TIMER1_COMPA_vect) is called when TCNT1=OCR1A(22499 in this case).
        a=true;                                               //Execute the corresponding code in void loop().
}
        
unsigned int getMeasurement(char x) {
        unsigned int cm=0;
        switch(x)  {
              case 1:
                       cm=sonar1.ping_cm();                   //Ping the signal, calculate the time and return the value in cm for sensor1.
                       break;
              case 2:
                       cm=sonar2.ping_cm();                   //Same for sensor2
                       break;
              case 3:
                       cm=sonar3.ping_cm();                   //Same for sensor3
                       break;        
                   }
        return(cm);
}
