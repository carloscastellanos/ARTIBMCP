autowatch=1; // js object should watch this file and reload it if it changes

function setLoc(w, h) {
	screenWidth = w; // screen width
	screenHeight = h; // screen height
	pWinSize = this.patcher.wind.size; // the current patcher window size (w, h)
	//currLoc = this.patcher.wind.location; // left, top, right, bottom
	left = (screenWidth - pWinSize[0]) / 2;
    top = (screenHeight - pWinSize[1]) / 2;
	right = left + pWinSize[0];
	bottom = top + pWinSize[1];
	
	//this.patcher.wind.setlocation(left,top,right,bottom);
	this.patcher.wind.location = [left,top,right,bottom];
	
	post(left,top,right,bottom);
	
	return true;
}

function fullScreen(val) {
	p = this.patcher;
	if(val>=1) {
		p.fullscreen(1);
	} else {
		p.fullscreen(0);
	}
}

function presentationMode(val) {
	p = this.patcher;
	if(val>=1) {
		p.presentation(1);
	} else {
		p.presentation(0);
	}
}

function setTitleBar(val) {
	p = this.patcher;
	if(val>=1) {
		p.wind.hastitlebar=1;
	} else {
		p.wind.hastitlebar=0;
	}
}


// ================================================================

function moveIt(objName, x, y)
{
	obj = this.patcher.getnamed(objName)
	var width,height;
	var r = new Array();
	width  = obj.rect[2] - obj.rect[0];
	height = obj.rect[3] - obj.rect[1];
	r[0] = x;
	r[1] = y;
	r[2] = x+width;
	r[3] = y+height;
	
	obj.rect = r;
	
	return true;
}
moveIt.local=1; // make private
