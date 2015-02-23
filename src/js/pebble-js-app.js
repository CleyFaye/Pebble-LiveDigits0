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
    Pebble.openURL('https://rawgit.com/CleyFaye/Pebble-LiveDigits0/b8de0568d6c0b526b16325dee082908ef0d28e5a/html/livedigits0.htm' + optString);
});

Pebble.addEventListener("webviewclosed", function(e) {
    // webview closed
    // Using primitive JSON validity and non-empty check
    if (e.response.charAt(0) == "{" && e.response.slice(-1) == "}" && e.response.length > 5) {
        var dialogString = decodeURIComponent(e.response);
        var cfg;
        if (isNewVersion()) {
            cfg = JSON.parse(dialogString);
        } else {
            cfg = {};
            var dialogCfg = JSON.parse(dialogString);
            var savedCfg = JSON.parse(JSON.parse(localStorage.liveDigits0).config);
            for (var key in dialogCfg) {
                if (savedCfg[key] != dialogCfg[key]) {
                    cfg[key] = dialogCfg[key];
                }
            }
        }
        localStorage.liveDigits0 = JSON.stringify({"version": appVersion, "config": dialogString});
        Pebble.sendAppMessage(cfg);
    }
});
