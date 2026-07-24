module.exports = require("./build/Debug/addon.node");
const addon = require("./");

//for now get value only returns string
console.log(addon.GetValue("gear"));
console.log(addon.GetValue("velocity"));
