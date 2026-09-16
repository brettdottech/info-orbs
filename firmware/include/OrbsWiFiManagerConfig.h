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

const char WEBPORTAL_CUSTOM_IMAGES_PAGE[] = R"HTML(
<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title>InfoOrbs Custom Images</title>
    <style>
        :root { color-scheme: dark; --blue:#1fa3ec; --panel:#151515; --muted:#a8a8a8; }
        * { box-sizing: border-box; }
        body { margin:0; background:#060606; color:#fff; font-family:Verdana,sans-serif; }
        main { width:min(1050px,100%); margin:auto; padding:20px; }
        h1 { margin-bottom:8px; }
        .muted { color:var(--muted); line-height:1.5; }
        .toolbar { display:flex; flex-wrap:wrap; gap:14px; align-items:end; padding:16px;
                   background:var(--panel); border-radius:10px; margin:20px 0; }
        label { display:block; font-size:.9rem; margin-bottom:6px; }
        input[type=number], select { min-height:42px; padding:8px 10px; color:#fff; background:#292929;
                                    border:1px solid #555; border-radius:6px; font-size:1rem; }
        input[type=number] { width:100px; }
        .grid { display:grid; grid-template-columns:repeat(auto-fit,minmax(170px,1fr)); gap:14px; }
        .card { padding:12px; background:var(--panel); border:1px solid #333; border-radius:10px; text-align:center; }
        canvas { display:block; width:100%; max-width:240px; aspect-ratio:1; margin:0 auto 10px;
                 background:#000; border:1px solid #444; border-radius:50%; }
        input[type=file] { display:none; }
        button, .pick { display:inline-block; padding:11px 16px; border:0; border-radius:6px;
                        color:#fff; background:var(--blue); cursor:pointer; font-size:1rem; }
        button:hover, .pick:hover { filter:brightness(.88); }
        button:disabled { opacity:.45; cursor:not-allowed; }
        .pick { width:100%; }
        .filename { height:34px; margin-top:8px; color:var(--muted); font-size:.78rem;
                    overflow-wrap:anywhere; }
        .actions { display:flex; gap:10px; margin-top:18px; }
        .actions button { flex:1; }
        .back { background:#444; text-decoration:none; text-align:center; }
        #status { min-height:24px; margin-top:14px; font-weight:bold; }
        .ok { color:#62d98b; } .error { color:#ff7777; }
        .progress { width:100%; height:8px; margin-top:12px; overflow:hidden; background:#333; border-radius:5px; }
        .bar { width:0; height:100%; background:var(--blue); transition:width .2s; }
        @media(max-width:520px) { main{padding:14px}.grid{grid-template-columns:1fr 1fr}.actions{flex-direction:column} }
    </style>
</head>
<body>
<main>
    <h1>Custom Images</h1>
    <p class="muted">Choose one image for each display. Images are resized in this browser to a
       240 &times; 240 baseline JPEG before upload. LittleFS: {{LITTLEFS_USED}} / {{LITTLEFS_TOTAL}} KiB used.</p>

    <div class="toolbar">
        <div>
            <label for="page">Image page (1-100)</label>
            <input id="page" type="number" min="1" max="100" value="1">
        </div>
        <div>
            <label for="fit">Image fitting</label>
            <select id="fit">
                <option value="cover">Fill screen (crop edges)</option>
                <option value="contain">Show whole image (black margins)</option>
            </select>
        </div>
        <p class="muted">Page 1 uses the original filenames. A page becomes active after it and every earlier page have all five slots.</p>
    </div>

    <div class="grid" id="grid"></div>

    <div class="progress"><div class="bar" id="bar"></div></div>
    <div id="status"></div>
    <div class="actions">
        <button id="upload" type="button">Resize and upload selected images</button>
        <a class="pick back" href="/">Back</a>
    </div>
</main>

<script>
(() => {
    const SIZE = 240;
    const slots = [];
    const grid = document.getElementById('grid');
    const pageInput = document.getElementById('page');
    const fitInput = document.getElementById('fit');
    const uploadButton = document.getElementById('upload');
    const status = document.getElementById('status');
    const bar = document.getElementById('bar');

    const fileNameFor = (page, screen) => page === 1
        ? `screen_${screen}.jpg`
        : `page_${page}_screen_${screen}.jpg`;
    const fileUrlFor = (page, screen) =>
        `/download?path=${encodeURIComponent('/CustomImageWidget/' + fileNameFor(page, screen))}&v=${Date.now()}`;

    function setStatus(text, type = '') {
        status.textContent = text;
        status.className = type;
    }

    function clearCanvas(canvas) {
        const ctx = canvas.getContext('2d');
        ctx.fillStyle = '#000';
        ctx.fillRect(0, 0, SIZE, SIZE);
        ctx.fillStyle = '#777';
        ctx.font = '16px sans-serif';
        ctx.textAlign = 'center';
        ctx.fillText('No image', SIZE / 2, SIZE / 2);
    }

    function drawFitted(source, canvas, mode) {
        const ctx = canvas.getContext('2d');
        ctx.fillStyle = '#000';
        ctx.fillRect(0, 0, SIZE, SIZE);
        const scale = mode === 'cover'
            ? Math.max(SIZE / source.width, SIZE / source.height)
            : Math.min(SIZE / source.width, SIZE / source.height);
        const width = source.width * scale;
        const height = source.height * scale;
        ctx.drawImage(source, (SIZE - width) / 2, (SIZE - height) / 2, width, height);
    }

    function loadImage(url) {
        return new Promise((resolve, reject) => {
            const img = new Image();
            img.onload = () => resolve(img);
            img.onerror = reject;
            img.src = url;
        });
    }

    async function previewSelected(slot) {
        if (!slot.file) return;
        const url = URL.createObjectURL(slot.file);
        try {
            const img = await loadImage(url);
            drawFitted(img, slot.canvas, fitInput.value);
            slot.name.textContent = slot.file.name + ' → 240×240 JPEG';
        } catch (_) {
            clearCanvas(slot.canvas);
            slot.name.textContent = 'This browser cannot decode that image';
        } finally {
            URL.revokeObjectURL(url);
        }
    }

    async function loadExisting() {
        const page = Math.max(1, Math.min(100, Number(pageInput.value) || 1));
        pageInput.value = page;
        setStatus('');
        for (const slot of slots) {
            slot.file = null;
            slot.input.value = '';
            try {
                const img = await loadImage(fileUrlFor(page, slot.screen));
                drawFitted(img, slot.canvas, 'contain');
                slot.name.textContent = fileNameFor(page, slot.screen) + ' (already uploaded)';
            } catch (_) {
                clearCanvas(slot.canvas);
                slot.name.textContent = 'Empty slot';
            }
        }
    }

    function canvasBlob(canvas) {
        return new Promise((resolve, reject) => canvas.toBlob(
            blob => blob ? resolve(blob) : reject(new Error('JPEG conversion failed')),
            'image/jpeg', 0.9));
    }

    async function convertedBlob(slot) {
        const url = URL.createObjectURL(slot.file);
        try {
            const img = await loadImage(url);
            drawFitted(img, slot.canvas, fitInput.value);
            return await canvasBlob(slot.canvas);
        } finally {
            URL.revokeObjectURL(url);
        }
    }

    async function uploadSlot(slot, page) {
        const blob = await convertedBlob(slot);
        const data = new FormData();
        data.append('/CustomImageWidget/', blob, fileNameFor(page, slot.screen));
        const response = await fetch('/upload', { method: 'POST', body: data });
        if (!response.ok) throw new Error(`Upload failed for screen ${slot.screen}`);
    }

    for (let screen = 1; screen <= 5; screen++) {
        const card = document.createElement('section');
        card.className = 'card';
        card.innerHTML = `<h3>Screen ${screen}</h3><canvas width="240" height="240"></canvas>
            <label class="pick">Choose image<input type="file" accept="image/*"></label>
            <div class="filename"></div>`;
        grid.appendChild(card);
        const slot = {
            screen,
            file: null,
            canvas: card.querySelector('canvas'),
            input: card.querySelector('input'),
            name: card.querySelector('.filename')
        };
        slot.input.addEventListener('change', () => {
            slot.file = slot.input.files[0] || null;
            previewSelected(slot);
        });
        slots.push(slot);
    }

    pageInput.addEventListener('change', loadExisting);
    fitInput.addEventListener('change', () => slots.forEach(previewSelected));
    uploadButton.addEventListener('click', async () => {
        const selected = slots.filter(slot => slot.file);
        if (!selected.length) {
            setStatus('Choose at least one image first.', 'error');
            return;
        }

        const page = Number(pageInput.value);
        uploadButton.disabled = true;
        bar.style.width = '0';
        try {
            for (let i = 0; i < selected.length; i++) {
                setStatus(`Converting and uploading screen ${selected[i].screen}...`);
                await uploadSlot(selected[i], page);
                bar.style.width = `${((i + 1) / selected.length) * 100}%`;
            }
            setStatus('Upload complete. The image page is ready when all five slots are present.', 'ok');
            await loadExisting();
            setStatus('Upload complete. The image page is ready when all five slots are present.', 'ok');
        } catch (error) {
            setStatus(error.message || 'Upload failed.', 'error');
        } finally {
            uploadButton.disabled = false;
        }
    });

    loadExisting();
})();
</script>
</body>
</html>
)HTML";

const char WEBPORTAL_BROWSE_HTML_START[] = R"(
<html>
<head>
    <title>InfoOrbs File Browser</title>
)";

const char WEBPORTAL_BROWSE_STYLE[] = R"(
<style>
    body { background: #060606; color: #fff; font-family: verdana; padding: 20px; }
    ul { list-style-type: disc; list-style-position: inside; padding: 0; }
    li { margin: 10px 0; align-items: center; }
    a { text-decoration: none; color: #fff; }
    a:hover { text-decoration: underline; }
    .button { display: inline-block; padding: 10px 20px; margin: 5px 0; background-color: #007BFF; color: white; border: none; border-radius: 5px; text-align: center; cursor: pointer; text-decoration: none; }
    .delete { margin: 5px 20px; }
    .button:hover { background-color: #0056b3; }
    .container { max-width: 800px; margin: auto; background: #111; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }
    .icon { cursor: pointer; margin-left: 10px; color: red; }
    .preview { width: 100px; height: 100px; object-fit: cover; margin-right: 10px; }
    .input-group { display: flex; margin: 10px 0; }
    .input-group input { flex: 1; padding: 10px; border: 1px solid #ccc; border-radius: 5px; margin-right: 10px; }
    .file-item {
        display: flex;
        align-items: center; /* Vertically centers the content */
        margin-bottom: 15px; /* Optional spacing between file items */
    }

    .file-item img.preview {
        margin-right: 15px; /* Adds spacing between the image and the text */
    }

    .file-item button {
        margin-left: auto; /* Pushes the delete button to the right */
    }
    input[type="file"] {
        display: none;
    }
    /* Overlay covering the whole page */
    .overlay {
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background-color: rgba(0, 0, 0, 0.5); /* Semi-transparent background */
        display: none; /* Hidden by default */
        z-index: 9998; /* Below spinner */
        justify-content: center;
        align-items: center;
    }
    /* Spinner styling */
    .spinner {
        text-align: center; /* Center the text below the spinner */
        z-index: 9999; /* Ensure it's above the overlay */
    }
    .spinner-circle {
        border: 4px solid #f3f3f3; 
        border-top: 4px solid #3498db; 
        border-radius: 50%; 
        width: 100px; 
        height: 100px; 
        animation: spin 2s linear infinite; 
        margin: 0 auto; /* Center the circle */
    }
    .spinner-text {
        margin-top: 10px; /* Space between the spinner and the text */
        font-size: 16px;
        color: #CCC;
    }
    @keyframes spin {
        0% { transform: rotate(0deg); }
        100% { transform: rotate(360deg); }
    }
    /* Add blur to the page content when overlay is active */
    .blur {
        filter: blur(5px);
        pointer-events: none; /* Disable all interactions */
        user-select: none; /* Prevent text selection */
    }
    #drop-area {
        border: 2px dashed #007BFF;
        border-radius: 5px;
        padding: 20px;
        text-align: center;
        margin: 20px 0;
        background-color: #333;
    }
    #drop-area.hover {
        border-color: #0056b3;
        background-color: #666;
    }
</style>
)";

const char WEBPORTAL_BROWSE_SCRIPT[] = R"(
<script>
    async function handleFetchFromUrl(event) {
        event.preventDefault(); // Prevent the default form submission behavior

        const form = document.getElementById('download-url-form');
        const urlInput = document.getElementById('urlInput').value;
        const dirInput = document.getElementById('dirInput').value;

        const formData = new FormData();
        formData.append('url', urlInput);
        formData.append('dir', dirInput);

        try {
            showSpinner();
            const response = await fetch(form.action, {
                method: 'POST',
                body: formData
            });
            hideSpinner();
            if (response.ok) {
                location.reload(); // Refresh the page to show the new files
            } else {
                const error = await response.text();
                alert('Error: ' + error);
            }
        } catch (error) {
            hideSpinner();
            alert('Download failed: ' + error.message);
        }
    }

    function confirmDelete(file, dir) {
        if (confirm('Are you sure you want to delete this file?')) {
            const url = `/delete?file=${encodeURIComponent(file)}&dir=${encodeURIComponent(dir)}`;
            showSpinner();
            fetch(url, { method: 'GET' })
                .then(response => {
                    if (response.ok) {
                        location.reload(); // Refresh the current page
                    } else {
                        alert('Failed to delete the file.');
                    }
                })
                .catch(error => {
                    alert('Error: ' + error.message);
                });
        }
    }
    function showSpinner(text = 'Processing, please wait...') {
        const overlay = document.getElementById('overlay');
        const spinnerText = document.getElementById('spinner-text');
        const container = document.getElementById('container');

        if (spinnerText) spinnerText.textContent = text;

        overlay.style.display = 'flex'; // Show the overlay
        container.classList.add('blur'); // Blur the page content
    }
    function hideSpinner() {
        const overlay = document.getElementById('overlay');
        const container = document.getElementById('container');

        overlay.style.display = 'none'; // Hide the overlay
        container.classList.remove('blur'); // Remove blur from the page content
    }
    function setSpinnerText(text) {
        const spinnerText = document.getElementById('spinner-text');
        if (spinnerText) {
            spinnerText.textContent = text;
        }
    }

    document.addEventListener('DOMContentLoaded', () => {
        const dropArea = document.getElementById('drop-area');
        const fileInput = document.getElementById('fileElem');
        const form = document.getElementById('upload-form');

        dropArea.addEventListener('dragover', (e) => {
            e.preventDefault();
            dropArea.classList.add('hover');
        });

        dropArea.addEventListener('dragleave', () => {
            dropArea.classList.remove('hover');
        });

        dropArea.addEventListener('drop', (e) => {
            e.preventDefault();
            dropArea.classList.remove('hover');
            const files = e.dataTransfer.files;
            handleFiles(files);
        });

        fileInput.addEventListener('change', (e) => {
            handleFiles(fileInput.files);
        });

        function handleFiles(files) {
            // Send files via FormData
            const formData = new FormData();
            for (const file of files) {
                // Use the path as name
                formData.append(form.elements['dir'].value, file);
            }
            uploadFiles(formData);
        }

        async function uploadFiles(formData) {
            showSpinner();
            try {
                const response = await fetch(form.action, {
                    method: 'POST',
                    body: formData
                });
                hideSpinner();
                if (response.ok) {
                    location.reload(); // Refresh the page
                } else {
                    alert('Error uploading files.');
                }
            } catch (error) {
                hideSpinner();
                alert('Upload failed: ' + error.message);

            }
        }
    });
</script>
)";

const char WEBPORTAL_BROWSE_BODY_START[] = R"(
</head>
<body>
    <div class='overlay' id='overlay'>
        <div class='spinner' id='spinner'>
            <div class='spinner-circle'></div>
            <p class='spinner-text' id='spinner-text'>Processing, please wait...</p>
        </div>
    </div>
    <div class='container' id='container'>
        <h2>InfoOrbs File Browser</h2>
)";

const char WEBPORTAL_BROWSE_UPLOAD_FORM1[] = "<h3>Upload new files to ";

const char WEBPORTAL_BROWSE_UPLOAD_FORM2[] = R"(:</h3>
        <div id='drop-area'>
            <form id='upload-form' method='POST' enctype='multipart/form-data' action='/upload'>
                <p>Drag and drop files here or <label class='button' for='fileElem'>Select files</label><input type='file' id='fileElem' name=')";

const char WEBPORTAL_BROWSE_UPLOAD_FORM3[] = R"(' multiple></p>
                <input type='hidden' name='dir' value=')";

const char WEBPORTAL_BROWSE_UPLOAD_FORM4[] = R"('>
            </form>
        </div>
)";

const char WEBPORTAL_BROWSE_FETCHURL_FORM1[] = R"(
        <h3>Fetch CustomClock images (0.jpg, ..., 11.jpg) from URL:</h3>
        <form id='download-url-form' action='/fetchFromUrl' onsubmit='handleFetchFromUrl(event)'>
            <div class='input-group'>
                <input type='text' id='urlInput' name='url' placeholder='Enter URL (e.g., http://example.com)' required>
                <input type='hidden' id='dirInput' name='dir' value=')";

const char WEBPORTAL_BROWSE_FETCHURL_FORM2[] = R"('>
            <button class='button' type='submit'>Fetch</button>
            </div>
        </form>
)";

const char WEBPORTAL_BROWSE_HTML_END[] = R"(
    </div>
</body>
</html>
)";

#endif // ORBS_WIFI_MANAGER_CONFIG_H
