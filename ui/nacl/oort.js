oort = null;  // Global application object.

// Indicate success when the NaCl module has loaded.
function moduleDidLoad() {
	oort = document.getElementById('oort');
	oort.postMessage('start');
}

// Handle a message coming from the NaCl module.
function handleMessage(message_event) {
	alert(message_event.data);
}
