# PebbleFirefighterBuddy
A pebble app to start a timer whenever a mobilization is accepted and fetch the intervention type, vehicle and address from the mobilization email

Ideally, this app would start by itself whenever the mobilization call is received (it's always the same number).
From the documentation, it seems that triggering actions when a special number calls is not possible and that calls don't add pins to the timeline.
I don't have a physical Pebble to check (yet :) I'm waiting on my Round 2 preorder) but it seems possible to automate this with either Pebble dialer (does it still works ?) or other Android automation apps.

To use the email fetching feature, you need to fill your email address informations in the server/check_mail.php file (on github) and host it somewhere.

TODO for V2:
 - add a way to specify the email server address in the pebble app settings section so that rebuilding the app is not necessary
 - allow scrolling back and forth in the logged time (maybe allow prenaming them in the app depending on when they are used (eg : left home, reached casern, vehicule leaving, on place, ... )) and log them on the timeline or in the app so that they are not lost when leaving the app (to use them for reports)


I have no idea if this can be usefull to anyone beside me but I'm still releasing it in the hope someone with a similar mobilization flow can adapt it or reuse the idea ! Fetching specific info from automated emails is a common need after all ! :)