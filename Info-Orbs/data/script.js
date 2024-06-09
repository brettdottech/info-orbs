const disableWidgetCheck = () => {
  const inputs = document.getElementsByTagName("input");
  const widgetSettings =
    Array.from(inputs).filter(
      (input) => input.type === "checkbox" && input.checked
    ) || null;
  if (widgetSettings && widgetSettings.length === 1) {
    widgetSettings[0].disabled = true;
    const widgetWarning = document.getElementById("widget-warning");
    widgetWarning.setAttribute("class", "widget-warning");
  } else {
    widgetSettings.forEach((setting) => {
      setting.disabled = false;
    });
  }
};

const onWidgetSettingChanged = (event) => {
  disableWidgetCheck();
};
