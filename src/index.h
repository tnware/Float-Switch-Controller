#ifndef INDEX_H
#define INDEX_H

const char *webpage = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>Float Switch Status</title>
    <link href="https://cdn.jsdelivr.net/npm/tailwindcss@2.2.19/dist/tailwind.min.css" rel="stylesheet">
    <style>
        .status-indicator {
            display: inline-block;
            width: 24px;
            height: 24px;
            border-radius: 50%;
            margin-right: 8px;
        }
        .status-okay {
            background-color: #10B981;
        }
        .status-alert {
            background-color: #EF4444;
        }
          .badge {
    display: inline-block;
    padding: 0.25em 0.6em;
    font-size: 75%;
    font-weight: 700;
    line-height: 1;
    text-align: center;
    white-space: nowrap;
    vertical-align: baseline;
    border-radius: 0.375rem;
  }
  .badge-okay {
    background-color: #10B981;
  }
  .badge-alert {
    background-color: #EF4444;
  }
    </style>
    <link href="https://cdn.materialdesignicons.com/5.4.55/css/materialdesignicons.min.css" rel="stylesheet">
</head>
<body class="bg-gray-900 text-white">
<div class="max-w-2xl mx-auto p-5">
    <h1 class="text-3xl font-bold my-6 text-center">Float Switch Status</h1>
    <div class="bg-gray-800 p-6 rounded-lg shadow-md">
<div class="flex justify-between items-center">
  <div class="flex items-center">
    <i class="mdi mdi-earth mdi-24px"></i>
    <p class="ml-2">Global Status: <span id="globalStatus" class="badge badge-okay">OKAY</span></p>
  </div>
  <div class="flex items-center">
    <i class="mdi mdi-flash mdi-24px"></i>
    <p class="ml-2">Relay Status: <span id="relayStatus" class="badge badge-alert">OFF</span></p>
  </div>
</div>

        <div id="switches"><!-- Switches will be loaded here --></div>
        <form action='/override' method='get' class="mt-4">
            <button class='w-full px-4 py-2 text-black font-bold bg-green-400 hover:bg-green-500 rounded-full shadow'>Toggle Override Mode</button>
        </form>
    </div>
</div>

<script>
function updateStatus() {
  fetch('/getRelayStatus')
    .then(response => response.json())
    .then(data => {
      const relayStatus = data.relayStatus;
      const relayStatusText = document.getElementById('relayStatus');
      relayStatusText.textContent = relayStatus;
      relayStatusText.className = relayStatus === 'ON' ? 'badge badge-okay' : 'badge badge-alert';
    });

  fetch('/getGlobalStatus')
    .then(response => response.json())
    .then(data => {
      const globalStatus = data.globalStatus;
      const globalStatusText = document.getElementById('globalStatus');
      globalStatusText.textContent = globalStatus;
      globalStatusText.className = globalStatus === 'OKAY' ? 'badge badge-okay' : 'badge badge-alert';
    });
}

function fetchSwitchData() {
    fetch('/getSwitchData')
        .then(response => response.json())
        .then(data => {
let switchesHTML = '';
for (let i = 0; i < Object.keys(data).length; i++) {
  let state = data[i];
  let buttonColor = state === 'CLOSED' ? 'bg-green-500 hover:bg-green-600' : 'bg-red-500 hover:bg-red-600';
  let statusClass = state === 'CLOSED' ? 'badge badge-okay' : 'badge badge-alert';
  switchesHTML += `
      <div class="flex items-center justify-between mb-2 p-2 bg-gray-700 rounded shadow">
          <div class="flex items-center">
              <span class="${statusClass}">${state}</span>
              <span class="ml-2">Switch ${i + 1}</span>
          </div>
          <form action='/toggle' method='get'>
              <button type='submit' name='switch' value='${i}' class='px-4 py-2 text-white font-bold ${buttonColor} rounded'>
                  Toggle
              </button>
          </form>
      </div>`;
}
document.getElementById('switches').innerHTML = switchesHTML;
        });
}


fetchSwitchData();
setInterval(fetchSwitchData, 5000); // Update every 5 seconds
updateStatus();
setInterval(updateStatus, 5000); // Update status every 5 seconds

</script>
</body>
</html>
)=====";

#endif
