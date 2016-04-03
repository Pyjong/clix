# clix
Utility for beating clicker games.

Using this .exe is pretty selfexplanatory. You click select window, click where you want it to click and then press Start/Stop. **Some clicker games require both the button_down and button_up messages.** You can set that by right clicking the frame of the dialog and then selecting messages in "clix msgs".

There is no limit to the number of clicks except for 1ms sleep after doing every click. Therefore it won't take away all your cpu time, **but** the games (especially in browser) may raise their hunger for cpu time because of all the message handling.

Some of the games I tried out:

* Zombidle - on Kongregate, doesn't need button up
* Cookie Clicker - does need button up
* Clicker Heroes - does need button up
* AdVenture captialist - Unity framework -> Zero f&@{s given, next!


Oh and btw.. the window selecting thing, it's set by losing focus on the clix window so.. don't alt-tab... I may revisit this later.
