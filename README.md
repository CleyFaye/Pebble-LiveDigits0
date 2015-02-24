# Pebble-LiveDigits0
A watchface for the Pebble smartwatch.

v1.4

Inspiration: this reddit thread
https://www.reddit.com/r/pebble/comments/2un40u/could_someone_put_this_animation_in_a_watchface/

Digit animation concept from /u/MattBaster
Watchface by me, /u/Cley_Faye

Current features:
 - Display the current time (hour/minutes) with animated digits (customizable speed/animation trigger, toggleable am/pm display)
 - Customizable layout
 - Invert color (by default use black background on dark pebble, white background on light pebble)
 - Customizable vibrate every hour/minute (with off-time)
 - Extra "widgets":
 - Display seconds in multiple way: a blinking dot, two static digit, or two animated digit (battery hungry!)
 - Display customizable date: either D/M, M/D, D, or M.
 - Display watch Bluetooth connectivity status. Can also vibrate when connectivity is lost with the phone.
 - Display watch battery status in two fashion: either a battery icon with 0-3 "bars", or the percentage left. Also indicate when the watch is plugged in.
 - All these widgets can either be always visible, or only appear for a short time when the watch is shaken a bit

Known bugs:
 - The watchapp consistently leak 68 bytes of heap when closed. Not an issue (the leak won't explode and the Pebble OS take care of that) but I'm puzzled. I know that 40 bytes of "leak" are caused by some timing functions, but the other 28 bytes are a mystery.
