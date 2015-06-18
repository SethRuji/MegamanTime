Pebble.addEventListener("ready",
											 function(e){
												 console.log("yo, JS is working");
												 Pebble.sendAppMessage({
													 "hello": 2
												 });
											 });


Pebble.addEventListener("appmessage",
											 function(e){
												 console.log("JS got a message");
											 });

Pebble.addEventListener("showConfiguration",
											 function(e){
												 Pebble.openURL("http://someoneseth.github.io/MegamanTime/");
											 });

Pebble.addEventListener("webviewclosed", function(e){
    console.log("Configuration closed");
    console.log("Response = " + e.response.length + "   " + e.response);
    if (e.response !== undefined && e.response !== '' && e.response !== 'CANCELLED') {
                console.log("User hit save");
                values = JSON.parse(decodeURIComponent(e.response));
                console.log("stringified options: " + JSON.stringify((values)));
                Pebble.sendAppMessage(
                        values
                );
        }
});