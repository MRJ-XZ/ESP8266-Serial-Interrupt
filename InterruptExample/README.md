## Serial interrupt with blinking LED
in this example, Serial.Read() is not used at all; instead, data is read directly from uart register. Updating **int recieved** is almost synchronous with the job that must be done in loop( in this example, blinking LED ). 
