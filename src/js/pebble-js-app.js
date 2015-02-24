var appVersion = 4;

function isNewVersion()
{
    try {
        cfgObj = JSON.parse(localStorage.liveDigits0);
        return cfgObj.version != appVersion;
    } catch (e) {
        return true;
    }
}

Pebble.addEventListener("ready", function() {
});

Pebble.addEventListener("showConfiguration", function() {
    var optString;
    if (isNewVersion()) {
        optString = '';
    } else {
        try {
            optString = '?'+encodeURIComponent(JSON.parse(localStorage.liveDigits0).config);
        } catch (e) {
            optString = '';
        }
    }
    var commit = 'f33e34f8a50779a801b492ef44ef67bf4a2a14ac';
    Pebble.openURL('https://cdn.rawgit.com/CleyFaye/Pebble-LiveDigits0/' + commit + '/html/livedigits0.htm' + optString);
});

Pebble.addEventListener("webviewclosed", function(e) {
    // webview closed
    // Using primitive JSON validity and non-empty check
    if (e.response.charAt(0) == "{" && e.response.slice(-1) == "}" && e.response.length > 5) {
        var dialogString = decodeURIComponent(e.response);
        var cfg;
        cfg = JSON.parse(dialogString);
        localStorage.liveDigits0 = JSON.stringify({"version": appVersion, "config": dialogString});
        Pebble.sendAppMessage(cfg);
    }
});
