// TODO Move into head_hidservice.js or similar
// e.g. https://dxr.mozilla.org/mozilla-central/source/toolkit/components/url-classifier/tests/unit/head_urlclassifier.js
var Cc = Components.classes;
var Ci = Components.interfaces;
var Cu = Components.utils;
var Cr = Components.results;

function run_test() {
    let hidService = Cc["@mozilla.org/dom/hid-service;1"].getService(Ci.nsIHidService);

    hidService.getDevices(function getDevicesCallback(aStatus, aDevices) {
        let hid_devices = [];
        while(aDevices.hasMoreElements())
        {
            let device = aDevices.getNext().QueryInterface(Ci.nsIHidDeviceInfo);
            dump("device.deviceId: " + device.deviceId + "\n");
            dump("device.vendorId: " + device.vendorId + "\n");
            dump("device.productId: " + device.productId + "\n");
            dump("device.manufacturerName: " + device.manufacturerName + "\n");
            dump("device.productName: " + device.productName + "\n");
            dump("device.serialNumber: " + device.serialNumber + "\n");
            dump("device.usage: " + device.usage + "\n");
            dump("device.usagePage: " + device.usagePage + "\n");
            dump("\n");

            if (device.usagePage == 0xf1d0 && device.usage == 0x0001) {
                dump("Added FIDO device\n");
                hid_devices.push(device);
            }
        }
    });
}
