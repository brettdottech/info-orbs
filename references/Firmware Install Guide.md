# Info Orbs Firmware Install Guide
Tip: If you're coming from the Youtube video, skip to step 11, as all the earlier steps for installing all the prerequisites are the same. The install process is still very similar, just some config settings and file locations have changed


1\. Download, unzip and install the CP210x drivers needed to connect the device to you computer from <https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads>

Ensure to select the correct operating system for the computer you're using.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/3e57b18f-592d-4f2c-9ea7-a520e7f1f723/ascreenshot.jpeg?tl_px=353,473&br_px=2073,1434&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,277)


2\. Ensure to go through the entire install process, it should be very straight forward

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/77c144b9-88c9-47b9-8efe-ea8b0483c56f/ascreenshot.jpeg?tl_px=0,56&br_px=1719,1018&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=611,11)


3\. Download, unzip and install the VSC code editor from <https://code.visualstudio.com/download>
Ensure to select the correct operating system for the computer you're using.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/ba402de3-e04f-4557-a1ba-888e2b8ff5b9/ascreenshot.jpeg?tl_px=975,738&br_px=2695,1699&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=700,290)


4\. Once installed, open VSC

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/925e83d6-ddd8-493e-9392-d07d68abd5ce/ascreenshot.jpeg?tl_px=916,358&br_px=2636,1319&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,277)


5\. Select the extensions menu on the left hand sidebar.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/31ff1d72-ad2f-4081-955f-b55205287398/ascreenshot.jpeg?tl_px=412,113&br_px=2132,1074&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=114,257)


6\. Search for "PlatformIO", and install it by clicking the install button in the searched list

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/c2ce1cc8-8af5-48aa-ade3-0254856c08ee/ascreenshot.jpeg?tl_px=378,0&br_px=2098,961&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,230)


7\. Fully close out of VSC.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/f0359c02-7113-48f8-9c50-949fc32343b4/ascreenshot.jpeg?tl_px=0,0&br_px=1719,961&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=366,34)


8\. Click the Address and search bar text field

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/e7f79acb-fe05-4b85-9a4e-b8df7918bf86/ascreenshot.jpeg?tl_px=234,0&br_px=1953,961&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,94)


9\. Navigate to <https://github.com/brettdottech/info-orbs> and download the latest firmware for the device by clicking on the green "Code" button, and the selecting "Download ZIP", then unzip that file

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/56e3fba6-4279-487c-a789-9d01960388fe/ascreenshot.jpeg?tl_px=629,564&br_px=2349,1526&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=515,438)


10\. Open the file you just downloaded

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/9a287f92-be87-4554-a437-dae00d6d7a65/ascreenshot.jpeg?tl_px=1220,0&br_px=2940,961&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=899,220)


11\. Open the file called"info-orbs.code-workspace"

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/6fae4791-547a-458b-95d7-16c68de64b62/ascreenshot.jpeg?tl_px=1098,225&br_px=2818,1186&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,277)


12\. Platform IO will need to configure a bunch of things in the background, this can take as long as 10 minutes, wait before this box at the bottom right is gone before you proceed

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/5bc6262a-a437-4438-9f92-2ea36407fca3/ascreenshot.jpeg?tl_px=1220,950&br_px=2940,1912&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=589,333)


13\. On the left hand menu, navigate to "config" file directory

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/99d75d03-15d3-4b98-b34f-26b7e2713199/ascreenshot.jpeg?tl_px=363,234&br_px=2083,1195&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=372,277)


14\. Rename the "config.h.template" file to just "config.h" (this is a critical step that will result in your code not being able to otherwise compile.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/07ecc161-a557-40f9-bb63-4e544571db31/ascreenshot.jpeg?tl_px=457,647&br_px=2177,1608&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=370,430)


15\. Open the config.h file by clicking on it, this is where your will configure your settings for the project, please feel free to play around with the many stylistic/preferential settings, however below are the critical ones: \
Change your time zone for the clock(s), you just replace the placeholder "American/Vancouver" with your local timezone identified from this list: [[[https://timezonedb.com/time-zones]]](https://timezonedb.com/time-zones)

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/0eb81e2f-3faa-4b4e-8e1f-d62c35648deb/ascreenshot.jpeg?tl_px=1187,8&br_px=2907,969&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,277)


16\. change the 24 hour clock format to true for a 24 hour clock, or remain it as false for a 12 hour clock

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/589f8434-0c53-40c1-873f-0b63fcd49091/ascreenshot.jpeg?tl_px=993,266&br_px=2713,1227&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,277)


17\. Configure your location for the weather widget using the format city,statecode

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/acde21c8-ddf4-4fcf-9372-6f769720ef27/ascreenshot.jpeg?tl_px=972,227&br_px=2692,1188&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,276)


18\. Set your 5 stocks to be displayed on the stock widget, entering all 5 in a single line as stated in the example, separated by commas.\
- To pull a stock from an international market you may add `&country=Germany` (or which ever country you prefer) to identify your local exchange
- If you're displaying forex or crypto, ensure to include `/USD` (or your given currency) as this will be needed to display the price.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/f69f1552-5b05-42f9-9a27-6b7c88c7dd18/ascreenshot.jpeg?tl_px=982,407&br_px=2702,1368&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,277)


19\. Now we're ready to flash the orbs with our firmware, first hold the "boot" button on your esp32

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/e66a8727-20e2-4b65-a920-7de97c24956b/screenshot.jpeg?tl_px=0,0&br_px=4032,3024&force_format=jpeg&q=100&width=1120.0)


20\. While holding the button, click the arrow symbol at the bottom of your VSC window

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/40600eb9-ddf9-4ae4-8171-1abbaa9e9b5c/ascreenshot.jpeg?tl_px=382,950&br_px=2102,1912&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,470)


21\. The code will take 15 seconds  -  1 minute to compile, keep holding down the boot button until you see "Writing at..." appear in the terminal window, at which point you can release the button

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/43c7b583-e022-4b04-88a7-501790097d88/ascreenshot.jpeg?tl_px=640,950&br_px=2360,1912&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,425)


22\. Once flashed, you should see this welcome screen, we now just need to configure our wifi credentials. \
Take note of the wifi network name your orbs are broadcasting, it will be Info-Orbs_xx

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/f2d09d27-cfe8-4f86-8a30-182aa537772e/screenshot.jpeg?tl_px=0,0&br_px=1232,792&force_format=jpeg&q=100&width=1120.0)


23\. Connect to that wifi network using a computer or phone.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/c6491aa6-ef28-4e32-98c8-05f45e88c010/user_cropped_screenshot.jpeg?tl_px=1220,39&br_px=2940,1000&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=725,277)


24\. You should see this window pop up when you connect to the wifi network (may take a few seconds)

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/7330b573-1c03-4daa-9a60-c70f692d4983/File.jpeg?tl_px=575,126&br_px=2295,1087&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,276)


25\. Should you be connected to the network and the window not pop up for some reason, you can navigate to the IP address listed on your orbs in a chrome browser as another means to access this window.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/ba998969-f6ba-4d33-ae77-e8e98e61ffeb/screenshot.jpeg?tl_px=0,0&br_px=1232,792&force_format=jpeg&q=100&width=1120.0)


26\. Click "Configure WiFi"

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/0d43914b-016f-457c-a5f0-d2d50d3c87e3/File.jpeg?tl_px=575,126&br_px=2295,1087&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,276)


27\. Enter your wifi credentials (a list of networks will also appear above to help aid in this), then click save.

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/95cc245e-948b-4521-8d36-4eb5fa0e8c72/user_cropped_screenshot.jpeg?tl_px=575,126&br_px=2295,1087&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,276)


28\. Your orbs are now connected to wifi and you are done!\
\
If you enter your credentials wrong, or if you ever want to change wifi networks, simply hold the far right button on your orbs while powering them on and it will reset the credentials, allowing you to access the wifi portal again (:

![](https://ajeuwbhvhr.cloudimg.io/colony-recorder.s3.amazonaws.com/files/2024-12-08/8835ffde-adcf-4a38-a0f4-dc2ef00956f0/File.jpeg?tl_px=627,89&br_px=2347,1050&force_format=jpeg&q=100&width=1120.0&wat=1&wat_opacity=0.7&wat_gravity=northwest&wat_url=https://colony-recorder.s3.us-west-1.amazonaws.com/images/watermarks/FB923C_standard.png&wat_pad=524,277)


