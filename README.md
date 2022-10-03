# rush-hour-computer-vision
Rush Hour puzzle solver for a certain mobile application using Scrcpy


## Instructions
Get [scrcpy](https://github.com/Genymobile/scrcpy) and follow the instructions there to display your device on the screen.
Do note that scrcpy is only available for android devices, therefore this app will *not* work for any other devices.

If you're having trouble, I found [this README](https://github.com/imneonizer/How-to-mirror-your-android-screen-with-opencv/blob/master/README.md) to be useful.

Next, make sure your phone displays the puzzle you want to be solved, and run the application. (Again, this application only works for a specific mobile app. If you want this to work for a different Rush Hour puzzle app, or if the actual board isn't being detected properly, feel free to fiddle around in `Vision.h`, `Vision.cpp` and `Source.cpp` ) 

Confirm the layout of the board is the same as the one shown in the console, and follow the steps.
