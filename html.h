
#ifndef HTML_H
#define HTML_H

const char DEFAULT_HTML[] = R"=====(
<!DOCTYPE html>
<html>
  <form action="/" method="POST">
    <input type="number" name="serialnumber" id="serialnumber" maxlength="16">
    <label for="serialnumber">Serial Number</label>
    <br>
    <input type="submit" value="Submit">
  </form>
</html>
)=====";

// *** change these strings to use const char str[] PROGMEM = R"=====(<html></html>)====="; to save dynamic memory
const String SSID_HTML = ""
"<!DOCTYPE html>"
"<html>"
  "<form action=\"/\" method=\"POST\">"
    "<input type=\"text\" name=\"ssid\" id=\"ssid\" maxlength=\"32\">"
    "<label for=\"password\">Network Name (SSID)</label>"
    "<br><input type=\"submit\" value=\"Submit\">"
  "</form>"
"</html>";

const char SSID_MENU_HTML_1[] = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <style>
      html {
        font-family: Helvetica;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      .button {
        background-color: #195B6A;
        border: none;
        color: white;
        padding: 16px 40px;
        text-decoration: none;
        font-size: 30px;
        margin: 2px;
        cursor: pointer;
      }
      .button2 {
        background-color: #77878A;
      }
      .dropButton {
        background-color: #3498DB;
        color: white;
        padding: 16px;
        font-size: 16px;
        border: none;
        cursor: pointer;
      }
      .dropButton:hover, .dropButton:focus {
        background-color: #2980B9;
      }
      .dropdown {
        position: relative;
        display: inline-block;
      }
      .dropdownContent {
        display: none;
        position: absolute;
        background-color: #f1f1f1;
        min-width: 160px;
        box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
        z-index: 1;
      }
      .dropdownContent a {
        color: black;
        padding: 12px 16px;
        text-decoration: none;
        display: block;
      }
      .dropdownContent a:hover {
        background-color: #ddd
      }
      .show {
        display: block;
      }
    </style>
    <script>
      function myFunction() {
        document.getElementById("myDropdown").classList.toggle("show");
      }
      window.onclick = function(event) {
        if (!event.target.matches('dropButton')) {
          var dropdowns = document.getElementsByClassName("dropdown-content");
          for (var i = 0; i < dropdowns.length; i++) {
            var openDropdown = dropdowns[i];
            if (openDropdown.classList.contains('show')) {
              openDropdown.classList.remove('show');
            }
          }
        }
      }
    </script>
  </head>
  <body>
    <div class="dropdown">
      <button onclick="myFunction()" class="dropButton">Select a Network</button>
      <div id="myDropdown" class="dropdown-content">
)=====";

const char SSID_MENU_HTML_2[] = R"=====(
        <button name="ssid" type="submit" value="other">other network</button>
      </div>
    </div>
  </body>
</html>
)=====";

const char PASSWORD_HTML[] = R"=====(
<!DOCTYPE html>
<html>
  <form action="/" method="POST">
    <input type="password" name="password" id="password" maxlength="64">
    <label for="password">Password</label>
    <br><input type="submit" value="Submit">
  </form>
</html>
)=====";

const char VERIFY_HTML[] = R"=====(
<!DOCTYPE html>
<html>
  <form action="/" method="POST">
    <button type="submit" name="verified" value="true">Looks Good!</button>
  </form>
</html>
)=====";

#endif
