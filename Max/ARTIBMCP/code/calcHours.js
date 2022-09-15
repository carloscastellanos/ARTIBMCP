// https://bobbyhadz.com/blog/javascript-get-number-of-hours-between-two-dates

autowatch=1

outlets=1

function getHoursDiff(year, month, day, hour, min, sec) {
	msInHour = 1000 * 60 * 60;
	
	startDate = new Date(year, month, day, hour, min, sec);
	currDate = new Date();

	//outlet(0, Math.round(Math.abs(currDate - startDate) / msInHour));
	outlet(0, Math.abs(currDate - startDate) / msInHour);
}
