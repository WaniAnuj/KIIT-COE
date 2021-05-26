var xhr = new XMLHttpRequest();
xhr.withCredentials = true;

xhr.addEventListener("readystatechange", function() {
  if(this.readyState === 4) {
    console.log(this.responseText);
  }
});

xhr.open("GET", "https://api.thingspeak.com/channels/1394967/feeds.json?api_key=WSXCBQW7MDOVN72W&results=1");

xhr.send();