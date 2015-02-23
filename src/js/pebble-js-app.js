var appVersion = 2;

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
    Pebble.openURL('https://cdn.rawgit.com/CleyFaye/Pebble-LiveDigits0/2a4a8b63cd6bf0a30404b54e190493849ddf2d99/html/livedigits0.htm' + optString);
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
