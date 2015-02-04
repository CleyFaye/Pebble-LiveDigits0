var options = {};

Pebble.addEventListener("ready", function() {
});

Pebble.addEventListener("showConfiguration", function() {
  Pebble.openURL('http://cleyfaye.net/pebble/livedigitsconfig.htm?'+encodeURIComponent(JSON.stringify(options)));
});

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Received message: ' + e.payload);
    options = e.payload;
  }
);

Pebble.addEventListener("webviewclosed", function(e) {
  // webview closed
  //Using primitive JSON validity and non-empty check
  if (e.response.charAt(0) == "{" && e.response.slice(-1) == "}" && e.response.length > 5) {
    options = JSON.parse(decodeURIComponent(e.response));
    Pebble.sendAppMessage(options);
  }
});