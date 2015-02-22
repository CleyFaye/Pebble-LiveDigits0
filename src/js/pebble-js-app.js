var appVersion = 1;

function isNewVersion()
{
    return localStorage.liveDigits0 === undefined || localStorage.liveDigits0.version === undefined || localStorage.liveDigits0.version != appVersion;
}

Pebble.addEventListener("ready", function() {
});

Pebble.addEventListener("showConfiguration", function() {
    var optString;
    if (isNewVersion()) {
        optString = '';
    } else {
        try {
            optString = '?'+encodeURIComponent(localStorage.liveDigits0.config);
        } catch (e) {
            optString = '';
        }
    }
    Pebble.openURL('http://cleyfaye.net/pebble/livedigits0.htm' + optString);
});

Pebble.addEventListener("webviewclosed", function(e) {
    // webview closed
    // Using primitive JSON validity and non-empty check
    if (e.response.charAt(0) == "{" && e.response.slice(-1) == "}" && e.response.length > 5) {
        var dialogString = decodeURIComponent(e.response);
        var cfg;
        if (isNewVersion() || localStorage.liveDigits0 === undefined) {
            cfg = JSON.parse(dialogString);
        } else {
            cfg = {};
            var dialogCfg = JSON.parse(dialogString);
            var savedCfg = JSON.parse(localStorage.liveDigits0);
            for (var key in dialogCfg) {
                if (savedCfg[key] != dialogCfg[key]) {
                    cfg[key] = dialogCfg[key];
                }
            }
        }
        localStorage.liveDigits0.version = appVersion;
        localStorage.liveDigits0.config = dialogString;
        Pebble.sendAppMessage(cfg);
    }
});
