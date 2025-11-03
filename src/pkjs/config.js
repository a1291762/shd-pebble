module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Here is some introductory text."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Face Settings"
      },
      {
        "type": "toggle",
        "messageKey": "DisplayBattery",
        "label": "Display Battery Percent",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "DisplaySeconds",
        "label": "Indicate Seconds",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
