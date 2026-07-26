const addon = require("./build/Release/addon.node");
//for now get value only returns string
(async function() {
    while(1){

    console.log(addon.GetValue("wheelsPressure"));
    
        await new Promise(r => setTimeout(r, 200));
    }
})();