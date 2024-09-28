# Description

This program monitors a few sample applications and makes sure that they are all running smoothly. If any of the applications stop working, the program alerts the user what application has shut down, and at what time.

## Approach

All the sample applications in this program will act the same. Whenever a sample application is started, it sends a registration message to the watchdog informing it of it's status. Then, every 10 seconds, it sends the watchdog a message indicating that it is still working as expected (heartbeat). If the watchdog doesn't recieve a message from one of the sample applications, it knows it has stopped working and it alerts the user.
    - The only job of the sample application is to send messages to the watchdog. 

I will use a shared named pipe for the watcdog that will be recieve messages from the applications. The sample applications will write their messages to this pipe including their unique id (process id). The watchdog will have two threads that it will use, one to recieve messages and alert the user, and one to check if any of the applications have not sent a heartbeat back. This will be a done uing an ordered map. The key will be the unique id and the value will be the time stamp. Evy 1 minute, it will check the timestamps and make sure the last heartbeat was within the last 10 seconds. If not, the program will alert the user.

The sample application will all send the same message to the watchdog through its pipe but each application will have a unique id to distinguish it from the other applications. When the pplication starts it will send the registration message using the watchdog pipe along with its id. Then it will enter aloop where it sends a status message every 10 seconds.

FIFO is a scpecial type of ile that is used  as a named pipe. It is a special kind of file that allows two different processes to communicate with each other by reading and writing to and from this file.