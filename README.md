# Spark-Of-Imagination
A senior design capstone project for making the Engineering building more magical

To run code to a specific effect, move the correctly named CPP file from effects into the SRC folder and then upload that to the specific effects ESP. 

The trigger_server.py file is to be ran on a Raspberry pi connected to a projector. The two .gif files need to be stored in the correct directory to make them play
To connect the ESP and Raspberry Pi, they both need to be connected to the same wifi network. The wifi credentials can be added to the remote under the SSID and Password variables on a WPA2 secured network.

## Adding Additional Effects
To add additional **ESP-NOW** effects, follow these steps:
1) create an additional CPP file for that effect
2) copy the ESP-NOW information into that file
    a) add an additional count and incomingDat variable
    b) inside of the loop, change the incomingDat to the variable you just created
3) on the remote file, add those new variables as well as a new buttonCount, a new incomingReading, and a new OutgoingMessage
4) create a new if-then conditon inside of the remote loop for the new ButtonCount to update the count when the remote button is pressed, this should say if the button count is N do (whatever you want it to do when the button is pushed). based on the size of the if condition, you can change how many button pushes affect that effect. (<4 = 4 button pushes)
5) add a new LED on the remote for your new effect by setting the previous LED to Black (off) and the effect your on to white(On) when the effect is off and your prepping for the next effect set it to orange (More details in color codes below)

You can add up to **19** effects using ESP-NOW. The remote can currently handle **8** effects with **indicator lights** (due to the length of the LED Strip attached to it) This strip can be extended or replaced by desoldering the strip and attaching a longer one or more lights can be soldered to the end of the strip. 

## Color Codes

White: Effect is active
Orange: Effect is Inactive
Blue: ESP-NOW is ready
Green: Successful POST to flask server on Raspberry Pi
Red: Unsuccessful POST to flask server on Raspberry Pi
