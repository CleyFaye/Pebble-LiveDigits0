/* Transition time between the three panes (loading, message, config) */
var transitionTime = 600;

/* Build the "result" of the config page */
function saveOptions() {
    var options = {}
    /* Add all textual values */
    $('textarea,select,[type="hidden"],[type="password"],[type="text"]').each(
            function() {
                if ($(this).is('.ui-slider-switch')) {
                    options[$(this).attr('id')] = ($(this).val() == 'on') ? '1' : '0';
                } else {
                    options[$(this).attr('id')] = $(this).val();
                }
            });
    /* Add all checkbox type values */
    $('[type="checkbox"]').each(
            function() {
                options[$(this).attr('id')] = $(this).is(':checked') ? '1' : '0';
            });
    /* Add all radio type values */
    $('[type="radio"]').each(
            function() {
                if ($(this).is(':checked')) {
                    options[$(this).attr('name')] = $(this).attr('value');
                }
            });
    return options;
}
function handleChange() {
    if (typeof doHandleChange === 'function') {
        doHandleChange();
    }
}
/* Load initial values from the URL.
 *
 * If no data are found in the URL, return false. true otherwise.
 */
function loadOptions() {
    var input = window.location.search.substring(1);
    var options;
    if (input.length == 0) {
        options = loadDefaults();
    } else {
        options = jQuery.parseJSON(decodeURIComponent(input));
    }
    for (var key in options) {
        var id = '#' + key;
        if ($(id).length == 0) {
            // Probably a radio
            $('input[name="' + key + '"][value="' + options[key] + '"]').attr('checked', true).checkboxradio('refresh');
            $('[name=' + key + ']').bind('change', handleChange);
        } else {
            if ($(id).is('.ui-slider-switch')) {
                $(id).val((options[key] == '1') ? 'on' : 'off');
                $(id).slider('refresh');
            } else if ($(id).is('select')) {
                $(id).val(options[key]);
                $(id).selectmenu('refresh');
            } else if ($(id).is('[type="checkbox"]')) {
                $(id).attr('checked', options[key] == '1');
                $(id).checkboxradio('refresh');
            }
            $(id).bind('change', handleChange);
        }
    }
    return true;
}
function onReady() {
    /* Close string */
    var cs = 'pebblejs://close';
    $('#b-cancel').click(function() {
        document.location = cs;
    });
    $('#b-submit').click(function() {
        var options = saveOptions();
        var json = JSON.stringify(options);
        var location = cs + "#" + encodeURIComponent(json);
        document.location = location;
    });
    $('#force_button').click(function() {
        $('#config_box').show(transitionTime);
        $('#message_box').hide(transitionTime);
    });
    /* Set form values to whatever is passed in */
    if (loadOptions()) {
        $('#config_box').show(transitionTime);
    } else {
        $('#message_box').show(transitionTime);
    }
    $('#loading_box').hide(transitionTime);
    $('#tabs').tabs({active: 0});
    handleChange();
}
$().ready(onReady);
