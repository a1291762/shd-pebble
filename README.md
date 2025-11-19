This is a watch face based on the loading animation from Tom Clancy's The Division.

Originally implemented for Wear OS (https://github.com/a1291762/shd/), ported to Pebble OS.

Features:

* Shows time in 12/24 hour format (respects user preference)
* Shows the date, including day of week
* Has an outer circle designed for displaying battery percentage
* Has 3 inner circle segments designed for displaying fitness information

As I was attempting to re-create the experience I had on my TicWatch E3, I
have setup the health display using the same logic. The top segment counts
active hours, where an active hour is defined as an hour where you take 100
steps. The left segment counts minutes of exercise, as tracked by the workout
app (manually entered, or automatically detected). The right segment tracks
steps for today. Goals can be set in the app preferences.

My Pebble 2 Duo does not fully support Pebble Heath so the separate goal
settings may overlap with system-provided goal settings. If/once I can set
them, then I'll remove the app preferences.

Third Party resources

Alarm Clock Font by David J Patterson https://www.dafont.com/alarm-clock.font

SHD Logo (found on the internet - ownership unclear) https://www.reddit.com/r/thedivision/comments/sbegag/i_had_my_wife_remake_the_shd_logo/
