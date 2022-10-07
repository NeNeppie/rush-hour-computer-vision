# rush-hour-computer-vision
Rush Hour puzzle solver for a certain mobile application using Android Debug Bridge. Works only for Android devices, and on Windows.



## What's Rush Hour?
> Rush Hour is a sliding block puzzle invented by Nob Yoshigahara in the 1970s. The goal of the game is to get only the red car out through the exit of the board by moving the other vehicles out of its way. However, the cars and trucks obstruct the path which makes the puzzle even more difficult. Cars and trucks are both one square wide, but cars are two squares long and trucks are three squares long. Vehicles can only be moved along a straight line on the grid; rotation is forbidden.

Thanks, Wikipedia, very cool



## Instructions
* On your android device, enable *Developer Options* (follow the link below if you're having trouble).
* Enable [USB Debugging](https://developer.android.com/studio/command-line/adb#Enabling). You may need to enable a second setting to allow simulating input.
* Download adb (can also be found in the link above) or simply use the one provided in this repository. Don't forget the dll files. If needed, update `Exec.h` with the path to `adb.exe`
* If you'd like to solve multiple puzzles in succession, you will need an ad-blocker. Any will do, as long as it blocks any ad inbetween levels.
* Optional: To see the exact coordinates of your inputs, you can enable another Developer Option called "Pointer Location" (name may vary). Very useful in identifying issues.


Next, make sure your phone displays the puzzle you want solved, and run the application.

Disclaimer: for the time being, this application only works for a specific mobile app (Essentially, if you know you know). If you want this to work for a different Rush Hour puzzle app, or if the actual board isn't being detected properly, feel free to fiddle around in `Vision.h` and `Vision.cpp`
