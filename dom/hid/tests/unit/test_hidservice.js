// TODO Move into head_hidservice.js or similar
// e.g. https://dxr.mozilla.org/mozilla-central/source/toolkit/components/url-classifier/tests/unit/head_urlclassifier.js
var Cc = Components.classes;
var Ci = Components.interfaces;
var Cu = Components.utils;
var Cr = Components.results;

function run_test() {
  // Wait for async to run to completion
  do_test_pending();

  let hidService = Cc["@mozilla.org/dom/hid-service;1"].getService(Ci.nsIHidService);
  hidService.getDevices(function getDevicesCallback(aStatus, aDevices) {
    //dump("Inside the JS getDevicesCallback");
    while(aDevices.hasMoreElements()) {
      let device = aDevices.getNext().QueryInterface(Ci.nsIHidDeviceInfo);
      dump("Enumerated a HID device\n");
    }
    do_test_finished();
  });
}

/*
function run_test() {
  let hidService = Cc["@mozilla.org/dom/hid-service;1"].getService(Ci.nsIHidService);

  hidService.getDevices(function getDevicesCallback(aStatus, aDevices) {
    let u2f_devices = [];
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
        u2f_devices.push(device);
      }
    }

    if (u2f_devices.length > 0) {
      let u2f_device = u2f_devices[0];
      hidService.connect(u2f_device, function connectCallback(aStatus, aConnection) {
        dump("connectCallback aStatus: " + aStatus + "\n");
        if (aStatus === Cr.NS_OK) {
          do_test_pending();
          dump("Waiting 3 seconds...");
          let timer = Cc["@mozilla.org/timer;1"].createInstance(Ci.nsITimer);
          let event = {
            notify: function(timer) {
              dump("Done waiting");
              aConnection.disconnect(function disconnectCallback(aStatus) {
                dump("Disconnected HID device, aStatus: " + aStatus);
              })
              do_test_finished();
            }
          }
          timer.initWithCallback(event, 5000, Ci.nsITimer.TYPE_ONE_SHOT);
        }
      });
    } else {
      dump("No U2F devices found! ;_;");
    }
  });
}
*/
