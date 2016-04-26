# clix
Utility for beating clicker games.

Using this .exe is pretty selfexplanatory. You click select window, click where you want it to click and then press Start/Stop.

I added Settings, which you can find in the menu when you right click the window frame. In the settings you can setup hotkeys and upper limit for clicks per second. Also some games require both button_down and button_up messages to recognize a click. You can set that up in settings too.

The actual amount of clicks is calculated based on clock tick duration on your system (client versions of Windows have default of 15ms). The clock tick duration is value calculated with respect to your processor frequency. Clix takes the upper limit from settings and divides it by tick duration, "adds 1" and that is the final amount of clicks per being scheduled once. If nothing more important occurs in the system, added all up this makes your limit amount of clicks in a second. Therefore it won't take away all your cpu time, **but** the games (especially in browser) may raise their hunger for cpu time because of all the message handling.

There is no way to guarantee any minimum being met from user space. Trust MS kernel team, they're pretty good.

Some of the games I tried out:

* Zombidle - on Kongregate, doesn't need button up
* Cookie Clicker - does need button up
* Clicker Heroes - does need button up
* AdVenture captialist - Unity framework -> It works weird, I'm sorry. Buy me C# deconstructed book and I will figure this one out for you.


Oh and btw.. the window selecting thing, it's set by position of mouse when losing focus on the clix window so.. alt-tab sets window as well as clicking outside ( :(( I know.. but hey Spy++ can't do much better so.. there is my excuse).
