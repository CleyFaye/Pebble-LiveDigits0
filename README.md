# Pebble-LiveDigits0
A watchface for the Pebble smartwatch.

Inspiration: this reddit thread
https://www.reddit.com/r/pebble/comments/2un40u/could_someone_put_this_animation_in_a_watchface/

Digit animation concept from /u/MattBaster
Watchface by me, /u/Cley_Faye

Current features:
 - Display the current time with animated digits (yay)
 - Customization options available in the Watchface setting (through the Pebble App)
 - Option for white background/black digits
 - Option to display random digits animation when opening the watchface
 - Option to display random digits animation when tapping/shaking the Pebble
 - Option to skip silly time. For example, going from minute 59 to minute 60, 70, 80, 90, 00.

Future features:
 - Animation speed setting
 - Display current date in one of the empty corner
 - Display a "second" tick / counter somewhere.
 - Alternative orientation (hour in the top-right corner, minutes in the bottom-left)

Possible future features (but probably not going to happen):
 - Adding some elements (something to display the seconds, something to display the current date...)

Known bugs:
 - The configuration dialog forget the settings every time it is open. It doesn't affect the watchapp though, as settings are saved correctly.
 - If the pebble is set to 12h instead of 24h, the second digit for hours can show the animation going from 1 to 2 before looping back to 0.
 - Sometime, the animation freeze. It is unfrozen the next minute. Very rare occurence.
 - The watchapp consistently leak 68 bytes of heap when closed. Not an issue (the leak won't explode and the Pebble OS take care of that) but I'm puzzled. I know that 40 bytes of "leak" are caused by some timing functions, but the other 28 bytes are a mystery.
