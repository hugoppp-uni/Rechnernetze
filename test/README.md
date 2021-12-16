# Testing environment for HTTP Requests
Zum Einfachen Testen von HTTP-Requests kann ein API-Tester verwendet werden.\
In unserem Fall ist die Anwendung der Browser-Extension "Talend API Tester beschrieben".\

## Download Talend API Tester
[Talend API Tester](https://chrome.google.com/webstore/detail/talend-api-tester-free-ed/aejoelaoggembcahagimdiliamlcdmfm) in Google Chrome oder darauf basierendem Browser wie z. B. Brave als Extension aus dem Chrome Web Store herunterladen.\

## Test-Szenario importieren
- In Chrome den API-Tester öffnen
- Import -> API Tester Repository wählen und die Datei [http_server.json](/test/http_server.json) aus dem Ordner **test** importieren.

## Test-Szenarion ausführen
- Ggf. die Umgebungsvariablen der IP-Adresse und den Port des HTTP-Servers ändern (oben rechts unter "Environments")
- Oben auf Scenarios klicken
- Alle Test Cases ausführen oder einzelne Requests