module.exports = function(minified) {
    var clayConfig = this;
    var _ = minified._;
    var $ = minified.$;
    var HTML = minified.HTML;

    function toggleHealthTargets() {
        var show = this.get();
        var showFunc = show ? (item) => { item.show(); } : (item) => { item.hide(); };
        clayConfig.getItemsByGroup('HealthTarget').map(showFunc);
    }

    function toggleColors() {
        var show = this.get();
        var showFunc = show ? (item) => { item.show(); } : (item) => { item.hide(); };
        clayConfig.getItemsByGroup('Color').map(showFunc);
        var showFunc = !show ? (item) => { item.show(); } : (item) => { item.hide(); };
        clayConfig.getItemsByGroup('NotColor').map(showFunc);
    }

    clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
        var DisplayHealthToggle = clayConfig.getItemByMessageKey('DisplayHealth');
        if (DisplayHealthToggle) {
            toggleHealthTargets.call(DisplayHealthToggle);
            DisplayHealthToggle.on('change', toggleHealthTargets);
        } else {
            // If the toggle isn't even visible, assume it is false
            var obj = {get:function(){return false;}};
            toggleHealthTargets.call(obj);
        }

        var UseColorToggle = clayConfig.getItemByMessageKey('UseColor');
        if (UseColorToggle) {
            toggleColors.call(UseColorToggle);
            UseColorToggle.on('change', toggleColors);
        } else {
            // If the toggle isn't even visible, assume it is false
            var obj = {get:function(){return false;}};
            toggleColors.call(obj);
        }
    });
};
