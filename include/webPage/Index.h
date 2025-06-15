const char Index[] PROGMEM = R"=(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Умная розетка</title>
  <link rel="stylesheet" href="styles.css">  
</head>
<body onload>
	<div id="loading">Загрузка данных...</div>
    <div id="content">
		<h1>Настройка Wi-Fi</h1>
		<form id="wifiForm">
      <label for="ssid">Сеть:</label><br>
      <select id="ssid" name="ssid"></select><br><br>
      <label for="password">Пароль:</label><br>
      <input type="password" id="password" name="password"><br><br>
		</form>
    <h1>Настройка Телеграм бота</h1>
    <form id="TgBotForm">
      <label for="Token">Токен бота:</label><br>
      <input type="text" id="Token" name="Token"><br><br>
      <label for="Users">Пользователи:</label><br>
      <input type="text" id="Users" name="Users"><br><br>
    </form>
		<button type="button" onclick="configureWifi()">Сохранить</button>
	</div>		
  <script>
    document.addEventListener('DOMContentLoaded', function() {
      Promise.all([GetWifiData(), ScanWifi(), GetBotTgData()])
        .then(() => {
          document.getElementById('loading').style.display = 'none';
          document.getElementById('content').style.display = 'block';
        })
        .catch(error => {
          console.error('Ошибка при загрузке данных:', error);
          document.getElementById('loading').textContent = 
            'Ошибка загрузки';
        });
    });

    
    function ScanWifi() {
      return fetch(`/ScanWifi`)
      .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
      })
      .then(data => {
        const select = document.getElementById("ssid");
        select.innerHTML = "";

        data.forEach(network => {
          const option = document.createElement("option");
          option.value = network.SSID;
          option.text  = network.SSID;
          select.appendChild(option);
        });
      })
    }

    function GetWifiData() {
      const ssid = document.getElementById('ssid');
      const password = document.getElementById('password');
      
      return fetch(`/GetWifiData`)
      .then(response => {
        if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
        return response.json(); // Жду JSON
      })
      .then(data => {
        ssid.value = data.SSID;
        password.value = data.Password;
      })
    }
	
    function GetBotTgData() {
      const token = document.getElementById('Token');
      const users = document.getElementById('Users');
      
      return fetch(`/GetTgBotData`)
      .then(response => {
        if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
        return response.json(); // Жду JSON
      })
      .then(data => {
        token.value = data.BotToken;
        users.value = data.Users;
      })
    }

    function configureWifi() {
      const ssid = document.getElementById('ssid').value;
      const password = document.getElementById('password').value;
      const token = document.getElementById('Token').value;
      const users = document.getElementById('Users').value;

      var IsDataInput = true;

      if (!ssid) {
        IsDataInput = false;
        alert("Введите имя сети");
      }

      if (!password) {
        IsDataInput = false;
        alert("Введите пароль")
      }

      if (!token) {
        IsDataInput = false;
        alert("Введите токен бота")
      }

      if (!users) {
        IsDataInput = false;
        alert("Введите пользователей")
      }

      if (IsDataInput) fetch(`/RegDeviceData?SSID=${ssid}&Password=${password}&Token=${token}&Users=${users}`);
    }
  </script>
</body>
</html>
)=";