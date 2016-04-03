# clix
Utility for beating clicker games.

Using this .exe is pretty selfexplanatory. You click select window, click where you want it to click and then press Start/Stop. **Some clicker games require both the button_down and button_up messages.** You can set that by right clicking the frame of the dialog and then selecting messages in "clix msgs".

There is no limit to the number of clicks except for 1ms sleep after doing every click. Therefore it won't take away all your cpu time, **but** the games (especially in browser) may raise their hunger for cpu time because of all the message handling.
