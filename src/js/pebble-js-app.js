Pebble.addEventListener("ready", function() {
});

Pebble.addEventListener("showConfiguration", function() {
    var optString;
    if (localStorage.liveDigits0 === undefined) {
        optString = '';
    } else {
        try {
            optString = '?'+encodeURIComponent(localStorage.liveDigits0);
        } catch (e) {
            optString = '';
        }
    }
    Pebble.openURL('https://cdn.rawgit.com/CleyFaye/Pebble-LiveDigits0/c3f2d7a7d899fab2a812e888b3d2c1df1143663f/html/livedigits0.htm' + optString);
});

Pebble.addEventListener("webviewclosed", function(e) {
    // webview closed
    // Using primitive JSON validity and non-empty check
    if (e.response.charAt(0) == "{" && e.response.slice(-1) == "}" && e.response.length > 5) {
        var dialogString = decodeURIComponent(e.response);
        var cfg;
        if (localStorage.liveDigits0 === undefined) {
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
        localStorage.liveDigits0 = dialogString;
        Pebble.sendAppMessage(cfg);
    }
});
