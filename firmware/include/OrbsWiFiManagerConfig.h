#ifndef ORBS_WIFI_MANAGER_CONFIG_H
#define ORBS_WIFI_MANAGER_CONFIG_H

#include <Arduino.h>

// If we want to override all strings from WiFiManager WebPortal, we can copy the file wm_strings_en.h to our include directory,
// give it a proper name and include it here
// #define WM_STRINGS_FILE "orbs_wifimanager_strings.h" // use this instead of wm_strings_en.h

const char WEBPORTAL_PARAM_PAGE_START[] = "<h1>InfoOrbs Configuration</h1>";
const char WEBPORTAL_PARAM_PAGE_END[] = "<br>"
                                        "<h3 style='color: red;'>Saving will restart the InfoOrbs to apply the new config.</h3>";
const char WEBPORTAL_PARAM_FIELDSET_START[] = "<fieldset>";
const char WEBPORTAL_PARAM_FIELDSET_END[] = "</fieldset>";
const char WEBPORTAL_PARAM_LEGEND_START[] = "<legend>";
const char WEBPORTAL_PARAM_LEGEND_END[] = "</legend>";
const char WEBPORTAL_PARAM_DIV_START[] = "<div class='param'>";
const char WEBPORTAL_PARAM_DIV_STRING_START[] = "<div class='param-string'>";
const char WEBPORTAL_PARAM_DIV_END[] = "</div>";
const char WEBPORTAL_PARAM_TOGGLE_ADVANCED[] = "<div class='toggle-adv'>Show Advanced Parameters</div>";
const char WEBPORTAL_PARAM_SPAN_ADVANCED_START[] = "<span class='adv-content'>";
const char WEBPORTAL_PARAM_SPAN_END[] = "</span>";

// Style for /param
const char WEBPORTAL_PARAM_STYLE[] = R"(
<style>
    .param { display: flex; align-items: center; margin: 0; }
    .param label { flex: 0 0 70%; text-align: left; }
    .param input { flex: 0 0 30%; text-align: right; }
    .param-string { margin: 0; }
    fieldset { margin: 10px 0; }
    legend { font-size: 1.5rem; font-weight: bold; }
    .toggle-adv {cursor: pointer; text-decoration: underline; }
    .toggle-adv:hover { font-weight: bold; }
    .adv-content { display:none; }
    .adv-content.open { display:block; }
</style>
)";

// JS for /param
const char WEBPORTAL_PARAM_SCRIPT[] = R"(
<script>
    document.querySelectorAll('.toggle-adv').forEach(toggle => {
        toggle.addEventListener('click', () => {
            const advContent = toggle.nextElementSibling;
            if (advContent.classList.contains('adv-content')) {
                advContent.classList.toggle('open');
                toggle.textContent = advContent.classList.contains('open') ? 'Hide Advanced Parameters' : 'Show Advanced Parameters';
            }
        });
    });
</script>
)";

// Start of /buttons
const char WEBPORTAL_BUTTONS_PAGE_START1[] = "<html>"
                                             "<head>";

const char WEBPORTAL_BUTTONS_PAGE_START2[] = R"(
</head>
<body>
    <div style='display: inline-block; min-width: 260px; max-width: 500px;padding: 5px;'>
    <h1>InfoOrbs Buttons</h1>
    <div class='info'>
        Here you can simulate button presses on the Orbs.
    </div>
    <div class='info'>
        The device supports 3 different events per button:<br>
        short (&lt;500ms), medium (500-2500ms) and long (&gt;2500ms)
    </div>
    <table>
)";

// End of /buttons
const char WEBPORTAL_BUTTONS_PAGE_END1[] = R"(
</table>
<form action='/' method='get'>
    <br><button class='back'>Back</button>
</form>
</div>
)";

const char WEBPORTAL_BUTTONS_PAGE_END2[] = "</body>"
                                           "</html>";

// Style for /buttons
const char WEBPORTAL_BUTTONS_STYLE[] = R"(
<style>
    body { background: #060606; text-align: center; color: #fff; font-family: verdana; }
    .info { margin: 10px 0; }
    table { text-align: center; width: 100%; }
    td { padding: 5px; }
    button.sim { height: 50px; width: 140px; border-radius: .3rem; }
    button.back { border-radius: .3rem; width: 100%; border: 0; background-color: #1fa3ec; color: #fff; line-height: 2.4rem; font-size: 1.2rem; }
</style>
)";

// JS for /buttons
const char WEBPORTAL_BUTTONS_SCRIPT[] = R"(
<script>
    function sendReq(name, state) {
        fetch(`/button?name=${name}&state=${state}`);
    }
</script>
)";

const char WEBPORTAL_BROWSE_START[] = R"(
<html>
<head>
    <title>InfoOrbs File Browser</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f4f4f9; margin: 0; padding: 20px; }
        h2, h3 { color: #333; }
        ul { list-style-type: none; padding: 0; }
        li { margin: 10px 0; display: flex; align-items: center; }
        a { text-decoration: none; color: #007BFF; }
        a:hover { text-decoration: underline; }
        .button { display: inline-block; padding: 10px 20px; margin: 5px 0; background-color: #007BFF; color: white; border: none; border-radius: 5px; text-align: center; cursor: pointer; text-decoration: none; }
        .delete { margin: 5px 20px; }
        .button:hover { background-color: #0056b3; }
        .container { max-width: 800px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }
        .icon { cursor: pointer; margin-left: 10px; color: red; }
        .preview { width: 100px; height: 100px; object-fit: cover; margin-right: 10px; }
        .input-group { display: flex; margin: 10px 0; }
        .input-group input { flex: 1; padding: 10px; border: 1px solid #ccc; border-radius: 5px; margin-right: 10px; }
        .spinner {
            border: 4px solid #f3f3f3; 
            border-top: 4px solid #3498db; 
            border-radius: 50%; 
            width: 100px; 
            height: 100px; 
            animation: spin 2s linear infinite; 
            display: none; /* Hidden by default */
            position: fixed; /* Fixed position relative to the viewport */
            top: 50%; /* Center vertically */
            left: 50%; /* Center horizontally */
            transform: translate(-50%, -50%); /* Offset by half its own size to perfectly center */
            z-index: 9999; /* Ensure the spinner is above other content */
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
    <script>
        function confirmDelete(file, dir) {
            if (confirm('Are you sure you want to delete this file?')) {
                window.location.href = '/delete?file=' + file + '&dir=' + dir;
            }
        }
        function showSpinner() {
                document.getElementById("spinner").style.display = "block";
        }
        function hideSpinner() {
            document.getElementById("spinner").style.display = "none";
        }
    </script>
</head>
<body>
    <div class="container">
        <h2>LittleFS File Browser</h2>
)";

const char WEBPORTAL_BROWSE_UPLOAD_FORM1[] = R"(
        <h3>Upload a new file</h3>
        <form method='POST' enctype='multipart/form-data' action='/upload'>
            <input type='file' name='file'>
            <input type='hidden' name='dir' value='
)";

const char WEBPORTAL_BROWSE_UPLOAD_FORM2[] = R"(
'>
            <input class='button' type='submit' value='Upload'>
        </form>
)";

const char WEBPORTAL_BROWSE_DOWNLOADURL_FORM1[] = R"(
        <h3>Download CustomClock images (1.jpg, ..., 11.jpg) from URL</h3>
        <form method='POST' action='/downloadFromUrl' onsubmit='showSpinner()'>
            <div class='input-group'>
                <input type='text' name='url' placeholder='Enter URL (e.g., http://example.com)' required>
                <input type='hidden' name='dir' value='
)";

const char WEBPORTAL_BROWSE_DOWNLOADURL_FORM2[] = R"(
'>
            <button class='button' type='submit'>Download</button>
            </div>
        </form>
        <div id="spinner" class='spinner'/>
    </div>
</body>
</html>
)";
#endif // ORBS_WIFI_MANAGER_CONFIG_H