
// Basic...
// Basic...
function getAnchor() {
    var reg_exp = new RegExp("#([^?&#]*)", "i").exec(location.hash);
    return reg_exp ? reg_exp[1] : "";
};

function getById(id) {
    return document.getElementById(id);
};

function getByTagName(element, tagName) {
    return element.getElementsByTagName(tagName);
};

function inner(element, content) {
    if (typeof content == "undefined") return element.innerHTML;
    element.innerHTML = content;
};

function outer(element, content) {
    if (typeof content == "undefined") return element.outerHTML;
    element.outerHTML = content;
};

function addLoading(msg_str) {
    getById("main").innerHTML = "<h2>" + msg_str + "</h2>";
};


// AjaxCom...
// AjaxCom...
var AjaxCom = new XMLHttpRequest();;

function AjaxOkHook(fn) {
    AjaxCom.onreadystatechange = function() {
        if(this.readyState == 4) {
            let Status = this.status;
            let RsText = this.responseText;
            if(Status == 200) {
                fn(RsText);
            }
        }
    }
};

function AjaxSend(post, url, arg) {
    AjaxCom.open((post ? "POST" : "GET"), url, true);
    AjaxCom.send(arg);
};

function AjaxAbort() {
    AjaxCom.abort();
};


// Main...
// Main...
function init() {
    window.onpopstate = function(event) { load(); };
};

function SetNavStatus() {
    var Anchor = getAnchor();
    var reg_exp = new RegExp("#([^?&#]*)", "i");
    var array = getByTagName(getById("navCont"), "a");
    if(!Anchor){
        Anchor = "home";
    }
    for(var i = 0; i < array.length; i++) {
        var aName = reg_exp.exec(array[i].href);
        if(aName) {
            if(Anchor == aName[1]) {
                array[i].style.backgroundColor = "#CC2828";
                array[i].onmouseout = function(){ this.style.backgroundColor = "#CC2828";};
            }else {
                array[i].style.backgroundColor = "";
                array[i].onmouseout = function(){ this.style.backgroundColor = "";};
            }
            array[i].onmouseover = function(){ this.style.backgroundColor = "#8e1c1c";};
        }
    }
};

function load(page_name) {
    let page_str;
    var iframeFlag = false;
    if(typeof page_name == "undefined") {
        var Anchor = getAnchor();
        if(Anchor){
            load(Anchor);
        }else{
            load("home");
        }
        return;
    }else {
        switch(page_name){
            case "home":
                page_str = "./home.shtml"; break;
            case "panel":
                page_str = "./panel.shtml"; break;
            case "command":
                page_str = "./command.shtml";break;
            case "settings":
                page_str = "./settings.shtml";break;
            case "login":
                page_str = "./login.shtml"; break;
            case "power":
                if(!HasLogin) {page_str = "./login.shtml"; break;}
                page_str = "./power.shtml"; 
                iframeFlag = true;
                break;
            case "list":
                if(!HasLogin) {page_str = "./login.shtml"; break;}
                page_str = "./ListPage.shtml";
                iframeFlag = true;
                break;
            case "delayer":
                if(!HasLogin) {page_str = "./login.shtml"; break;}
                page_str = "./DelayerPage.shtml";
                iframeFlag = true;
                break;
            default:
                addLoading("Page not found."); return;
        }
    }

    


    AddSess();
    SetLoginSt();
    SetNavStatus();
    if(iframeFlag == true) {
        document.getElementById("main_frame").hidden = true;
        document.getElementById("main").hidden = true;

        document.getElementById("main_frame").src= page_str;
        document.getElementById("main_frame").onload = function() {
            document.getElementById("main_frame").hidden = false;
        };
    }else{
        document.getElementById("main_frame").src= "";
        AjaxAbort();
        AjaxOkHook(function(Txt) {
            var element = null;
            var reg_exp = new RegExp("<body>([\\s\\S]*)<[/]body>");
            if(reg_exp.test(Txt)) {
                element = document.createElement("body");
                inner(element, reg_exp.exec(Txt)[1]);
            }
            if(element) {
                let MainElem = getById("main");
                let New_Main = getByTagName(element, "div")["main"];
                let scripts = getByTagName(element, "script");
                if(New_Main) {
                    inner(MainElem, inner(New_Main));
                }else {
                    inner(MainElem, inner(element));
                }
                for(var i = 0; i < scripts.length; i++) {
                    eval(inner(scripts[i]));
                }
                document.getElementById("main").hidden = false;
                document.getElementById("main_frame").hidden = true;
            }
        });
        AjaxSend(0, page_str, "");
    }
};


// Home...
// Home...
function ClickIdfy(Elm) {
    AjaxAbort();
    AjaxOkHook(function() {});
    AjaxSend(1, "/cgi/identify", Elm.checked?"idfy=1":"idfy=0");
};





//Panel...
//Panel...
var AjaxLcd = new XMLHttpRequest();
var AjaxLed = new XMLHttpRequest();
var DownKeyElem;
var KeyTimer = -1;
var TimeoutLcd = -1;
var TimeoutLed = -1;

function StartPanel() {
    AjaxLcd.abort();
    AjaxLed.abort();
    clearTimeout(TimeoutLcd);
    clearTimeout(TimeoutLed);
    StartLCD();
    StartLED();
};

function StartLCD() {
    AjaxLcd.abort();
    AjaxLcd.open("GET", "./panel/screen", true);
    AjaxLcd.responseType = "blob";
    AjaxLcd.onreadystatechange = function() {
        if(this.readyState == 4) {
            let LcdImg = getById("lcd");
            clearTimeout(TimeoutLcd);
            if(LcdImg) {
                if(this.status == 200) {
                    AddSess();
                    let Bolb = this.response;
                    if(Bolb.size > 0) {
                        let Reader = new FileReader();
                        Reader.onload = function() {
                            LcdImg.src = this.result;
                        };
                        Reader.readAsDataURL(Bolb);
                    }
                    TimeoutLcd = setTimeout(StartLCD, 100);
                }else if(this.status == 0) {
                    LcdImg.src = "data:image/gif;base64,R0lGODlhAQABAHAAACH5BAEAAAEALAAAAAABAAEAgQAAAAAAAAAAAAAAAAICTAEAOw==";
                    TimeoutLcd = setTimeout(StartLCD, 2000);
                }
            }
        }
    };
    AjaxLcd.send();
    clearTimeout(TimeoutLcd);
    TimeoutLcd = setTimeout(function(){ AjaxLcd.abort(); AjaxLed.abort(); }, 3000);
};

function StartLED() {
    AjaxLed.abort();
    AjaxLed.open("GET", "./panel/led", true);
    AjaxLed.onreadystatechange = function() {
        if(this.readyState == 4 && getById("panelCont")) {
            clearTimeout(TimeoutLed);
            if(this.status == 200 && this.responseText) {
                let json = JSON.parse(this.responseText);
                if(json.status == "OK") {
                    getById("HomeLed").style.backgroundColor = (json.home == 1) ? "#FFFFFF":"#999999";
                    getById("MenuLed").style.backgroundColor = (json.menu == 1) ? "#FFFFFF":"#999999";
                    getById("WaveLed").style.backgroundColor = (json.wave == 1) ? "#FFFFFF":"#999999";
                    getById("LockLed").style.backgroundColor = (json.lock == 1) ? "#FFFFFF":"#999999";
                    getById("Ch1Led").style.backgroundColor = (json.ch1 == 1) ? "#FFFF00":"#999999";
                    getById("Ch2Led").style.backgroundColor = (json.ch2 == 1) ? "#00FFFF":"#999999";
                    getById("Ch3Led").style.backgroundColor = (json.ch3 == 1) ? "#00FF00":"#999999";
                    getById("Ch4Led").style.backgroundColor = (json.ch4 == 1) ? "#FF00FF":"#999999";
                    getById("Ch1CcCvLed").style.backgroundColor = json.ch1st;
                    getById("Ch1OnOffLed").style.backgroundColor = (json.ch1st != "grey") ? "#FFFF00":"#999999";
                    getById("Ch2CcCvLed").style.backgroundColor = json.ch2st;
                    getById("Ch2OnOffLed").style.backgroundColor = (json.ch2st != "grey") ? "#00FFFF":"#999999";
                    getById("Ch3CcCvLed").style.backgroundColor = json.ch3st;
                    getById("Ch3OnOffLed").style.backgroundColor = (json.ch3st != "grey") ? "#00FF00":"#999999";
                    getById("Ch4CcCvLed").style.backgroundColor = json.ch4st;
                    getById("Ch4OnOffLed").style.backgroundColor = (json.ch4st != "grey") ? "#FF00FF":"#999999";
                }
                TimeoutLed = setTimeout(StartLED, 200);
            }else {
                let Leds = getByTagName(getById("panelCont"), "div");
                for(var i = 0; i < Leds.length; i++) {
                    Leds[i].style.backgroundColor = "#999999";
                }
                TimeoutLed = setTimeout(StartLED, 2000);
            }
        }
    };
    AjaxLed.send();
};

function PostKey(mode, key)
{
    KeyClear();
    AjaxAbort();
    AjaxOkHook(function(Txt) {
        var json = JSON.parse(Txt);
        if(json) {
            if(json.status == "ERR" && json.code == "AUTH") {
                load("login");
            }
        }
    });

    let keyarg;
    keyarg = "mode=" + mode;
    keyarg += "&key=" + key;
    AjaxSend(1, "/cgi/keyboard", keyarg);
};

function KeyClear() {
    clearTimeout(KeyTimer);
    KeyTimer = -1;
};

function KeyDown(KeyElem) {
    let Key = KeyElem.alt;
    DownKeyElem = KeyElem;
    if(KeyTimer < 0 && event.button == 0 && Key != "Panel") {
        KeyTimer = setTimeout(function(){ PostKey("long", Key);}, 1000);
    }
};

function KeyUp(KeyElem) {
    let Key = KeyElem.alt;
    if(KeyTimer > 0 && KeyElem == DownKeyElem && Key != "Panel") {
        PostKey("short", Key);
    }
    KeyClear();
};

function KeyLeave(KeyElem) {
    if(KeyElem == DownKeyElem) {
        KeyClear();
    }
};

function KeyTouchStr(KeyElem) {
    let Key = KeyElem.alt;
    if(KeyTimer < 0 && Key != "Panel") {
        KeyTimer = setTimeout(function(){ PostKey("long", Key);}, 1000);
    }
    event.preventDefault();
};

function KeyTouchEnd(KeyElem) {
    let Key = KeyElem.alt;
    if(KeyTimer > 0 && Key != "Panel") {
        PostKey("short", Key);
    }
    KeyClear();
    event.preventDefault();
};







//Panel Touch...
//Panel Touch...
var MouseIsDown = false;
var LastTouchX;
var LastTouchY;

function PostLcdTouch(mode, x, y)
{
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    if(x >= 480) x = 480 - 1;
    if(y >= 272) y = 272 - 1;

    AjaxAbort();
    AjaxOkHook(function(Txt) {
        var json = JSON.parse(Txt);
        if(json) {
            if(json.status == "ERR" && json.code == "AUTH") {
                load("login");
            }
        }
    });

    let touchArg;
    touchArg  = "mode=" + mode;
    touchArg += "&x=" + x;
    touchArg += "&y=" + y;
    AjaxSend(1, "/cgi/touchxy", touchArg);
};

function LcdMouseUp(LCD) {
    if(MouseIsDown && event.button == 0) {
        MouseIsDown = false;
        let X = Math.round(event.offsetX / LCD.clientWidth * 480);
        let Y = Math.round(event.offsetY / LCD.clientHeight * 272);
        PostLcdTouch(0, X, Y);
    }
};

function LcdMouseDown(LCD) {
    if(event.button == 0) {
        MouseIsDown = true;
        let X = Math.round(event.offsetX / LCD.clientWidth * 480);
        let Y = Math.round(event.offsetY / LCD.clientHeight * 272);
        PostLcdTouch(1, X, Y);
    }
};

function LcdMouseMove(LCD) {
    if(MouseIsDown && AjaxCom.readyState == 4) {
        let X = Math.round(event.offsetX / LCD.clientWidth * 480);
        let Y = Math.round(event.offsetY / LCD.clientHeight * 272);
        PostLcdTouch(1, X, Y);
    }
};

function LcdTouchStr(LCD) {
    let rect = LCD.getBoundingClientRect();
    let X = event.touches[0].clientX - rect.left;
    let Y = event.touches[0].clientY - rect.top;
    LastTouchX = Math.round(X / LCD.clientWidth * 480);
    LastTouchY = Math.round(Y / LCD.clientHeight * 272);
    PostLcdTouch(1, LastTouchX, LastTouchY);
    event.preventDefault();
};

function LcdTouchEnd(LCD) {
    PostLcdTouch(0, LastTouchX, LastTouchY);
    event.preventDefault();
};

function LcdTouchMove(LCD) {
    let rect = LCD.getBoundingClientRect();
    let X = event.touches[0].clientX - rect.left;
    let Y = event.touches[0].clientY - rect.top;
    LastTouchX = Math.round(X / LCD.clientWidth * 480);
    LastTouchY = Math.round(Y / LCD.clientHeight * 272);
    if(AjaxCom.readyState == 4) {
        PostLcdTouch(1, LastTouchX, LastTouchY);
    }
    event.preventDefault();
};











// SCPI...
// SCPI...
var ScpiRxTimoutCount;

function _ScpiRead() {
    AjaxAbort();
    AjaxOkHook(function(Txt) {
        if(Txt.length > 0) {
            let HstWin = getById("ScpiHistory");
            HstWin.value += "<- " + Txt;
            HstWin.scrollTop = HstWin.scrollHeight;
        }else if(ScpiRxTimoutCount < 1) {
            ScpiRxTimoutCount++;
            setTimeout(_ScpiRead, 200);
        }else {
            alert("Time out!");
        }
    });
    AjaxSend(0, "/cgi/scpi/read", "");
};

function ScpiSelCmd() {
    getById("ScpiCmd").value = getById("ScpiCmdSel").value;
    getById("ScpiCmdSel").value = "";
};

function ScpiWrite(fn, str) {
    let Cmd;
    if(typeof str != "undefined") {
        Cmd = str;
    }else {
        Cmd = getById("ScpiCmd").value;
    }
    if(!Cmd || Cmd == "") {
        return;
    }
    Cmd += "\n"

    let HstWin = getById("ScpiHistory");
    HstWin.value += "-> " + Cmd;
    HstWin.scrollTop = HstWin.scrollHeight;
    AjaxAbort();
    AjaxOkHook(function(Txt) {
        var json = JSON.parse(Txt);
        if(json) {
            if(json.status == "OK") {
                if(typeof fn != "undefined") {
                    fn();
                }
            }else if(json.code == "AUTH") {
                load("login");
            }else {
                alert("Busy!");
            }
        }
    });
    AjaxSend(1, "/cgi/scpi/write", "cgi_scpi=" + Cmd);
};

function ScpiRead() {
    ScpiRxTimoutCount = 0;
    _ScpiRead();
};

function ScpiQuery() {
    ScpiWrite(ScpiRead);
};

function ScpiRdErr() {
    ScpiWrite(ScpiRead, ":SYST:ERR?");
};

function ScpiClsHst() {
    getById("ScpiHistory").value="";
};











// Settings...
// Settings...
var IsEditLan;
var oldDhcp;
var oldIp;
var oldMask;
var oldGate;

function SetEditLanDhcp(dhcp) {
    let st;
    if(dhcp) {
        st = true;
    }else {
        st = false;
    }
    getById("set_dhcp").checked = st;
    getById("set_ip").disabled = st;
    getById("set_mask").disabled = st;
    getById("set_gate").disabled = st;
};

function ClickEditLan() {
    if(HasLogin) {
        if(!IsEditLan) {
            IsEditLan = true;
            getById("viewLan").hidden = true;
            getById("editLan").hidden = false;
            inner(getById("lanEditBtn"), "Cancel");
            getById("lanOkBtn").style.display = "inline-block";
            if(inner(getById("dhcp")) == "On") {
                oldDhcp = true;
            }else {
                oldDhcp = false;
            }
            oldIp = inner(getById("ip"));
            oldMask = inner(getById("mask"));
            oldGate = inner(getById("gate"));
            SetEditLanDhcp(oldDhcp);
            getById("set_ip").value = oldIp;
            getById("set_mask").value = oldMask;
            getById("set_gate").value = oldGate;
        }else {
            IsEditLan = false;
            getById("viewLan").hidden = false;
            getById("editLan").hidden = true;
            inner(getById("lanEditBtn"), "Edit");
            getById("lanOkBtn").style.display = "none";
        }
    }else {
        load("login");
    }
};

function ClickDhcp(Elm) {
    SetEditLanDhcp(Elm.checked);
};

function chkIp(str) {
    if(str.match(/^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/)) {
        return true;
    }
    return false;
};

function chkMask(str) {
    if(str.match(/^(((255\.){3}(252|248|240|224|192|128|0))|((255\.){2}(255|254|252|248|240|224|192|128|0)\.0)|((255\.)(255|254|252|248|240|224|192|128|0)(\.0){2})|((255|254|252|248|240|224|192|128|0+)(\.0){3}))$/)) {
        return true;
    }
    return false;
};

function ClickApplyLan() {
    let dhcp = getById("set_dhcp").checked;
    let ip = getById("set_ip").value;
    let mask = getById("set_mask").value;
    let gate = getById("set_gate").value;

    if(!dhcp) {
        if(!chkIp(ip)) {
            alert("Invalid IP address!");
            return;
        }
        if(!chkMask(mask)) {
            alert("Invalid subnet mask!");
            return;
        }
        if(!chkIp(gate)) {
            alert("Invalid default gateway!");
            return;
        }
    }
    if(!confirm("Confirm to apply the changes?")) {
        return;
    }

    IsEditLan = false;
    getById("viewLan").hidden = false;
    getById("editLan").hidden = true;
    inner(getById("lanEditBtn"), "Edit");
    getById("lanOkBtn").style.display = "none";

    AjaxAbort();
    AjaxOkHook(function(Txt) {
        var json = JSON.parse(Txt);
        if(json) {
            if(json.status == "OK") {
                addLoading("Network is being configured. Please reload this page in a few minutes.");
            }else if(json.status == "ERR" && json.code == "AUTH") {
                load("login");
            }else {
                alert("Error code:" + json.code);
            }
        }
    });

    let lanarg;
    if(dhcp) {
        lanarg = "netDHCP=true";
    }else {
        lanarg = "netDHCP=false";
        lanarg += "&netIPv4=" + ip;
        lanarg += "&netNetMask=" + mask;
        lanarg += "&netGateWay=" + gate;
    }
    AjaxSend(1, "/cgi/setlan", lanarg);
};

function ChkPw() {
    var oldPw = getById("oldPw");
    var newPw = getById("newPw");
    var CfrPw = getById("confirmPw");

    for(var i = 0; i < newPw.value.length; i++) {
        if( (newPw.value.charCodeAt(i) >= 48) && (newPw.value.charCodeAt(i) <= 57) ) {
        }else{
            alert("Password cannot contain non-Num character!");
            return(false);
        }
    }

    if(newPw.value !== CfrPw.value) {
        alert("\"Confirm Password\" and \"New Password\" do not match!");
        return(false);
    }

    ChgPw(oldPw.value, newPw.value);
    oldPw.value = null;
    newPw.value = null;
    CfrPw.value = null;

    return(true);
};

function ChgPw(OldPw, NewPw) {
    AjaxAbort();
    AjaxOkHook(function(Txt) {
        var json = JSON.parse(Txt);
        if(json) {
            if(json.status == "OK") {
                RemSess();
                load();
            }else {
                alert("The old password is incorrect!");
            }
        }
    });

    let arg = "oldPasswd=" + OldPw;
    arg += "&" + "newPasswd=" + NewPw;
    AjaxSend(1, "/cgi/changepasswd", arg);
};


// Login...
// Login...
//var HasLogin = false;
var HasLogin = true;
function SetLoginSt() {
    if(document.cookie.indexOf("id=") != -1) {
        HasLogin = true;
        inner(getById("loginout"), "Log out");
        getById("userhead").style.filter = "none";
    }else{
        HasLogin = false;
        inner(getById("loginout"), "Log in");
        getById("userhead").style.filter = "contrast(0%)";
    };
};

function TsetSess() {
    var NowTime = new Date();
    document.cookie = "test";
    if(document.cookie.indexOf("test") != -1) {
        document.cookie = "test; Expires=" + NowTime.toGMTString();
        return true;
    }else return false;
};

function AddSess(Id) {
    var time = new Date();
    time.setTime(Date.now() + 1000*60*30);
    var expired_time = "; Path=/; Expires="+time.toGMTString();
    if(typeof Id == "undefined") {
        var cookie = document.cookie;
        if(cookie.length > 0 && cookie.indexOf("id=") >= 0) {
            document.cookie = cookie+expired_time;
        }
    }else {
        document.cookie = "id="+Id+expired_time;
    }
};

function RemSess() {
    var time = new Date();
    document.cookie = "id=; Expires="+time.toGMTString();
};

function ClickLogInOut() {
    if(HasLogin) {
        SubmitLogout();
    }else {
        if(TsetSess()) {
            load("login");
        }else {
            addLoading("Login requires cookie. Please enable it and refresh this page.");
        }
    }
};

function SubmitLogin() {
    AjaxAbort();
    AjaxOkHook(function(Txt) {
        var json = JSON.parse(Txt);
        if(json) {
            if(json.status == "OK") {
                AddSess(json.code);
                parent.window.location.reload(); 
            }else {
                alert("The password is incorrect!");
            }
        }
    });

    let arg = "pass=" + getById("pwInput").value;
    getById("pwInput").value = null;
    AjaxSend(1, "/cgi/login", arg);
};

function SubmitLogout() {
    AjaxAbort();
    AjaxOkHook(function(Txt) {
        var json = JSON.parse(Txt);
        if(json) {
            if(json.status == "OK") {
                RemSess();
                load();
                alert("You have logged out!");
            }
        }
    });

    AjaxSend(1, "/cgi/logout", "");
};

function KeyLogin(KeyEv) {
    if(KeyEv.keyCode==13) {
        SubmitLogin();
    }
};

function MAX(a,b){
    if(a>b) return a;
    else return b;
}
function MIN(a,b){
    if(a>b) return b;
    else return a;
}

function httpsend_tag(method,src,sync,tag,data)
{
    try {
        var xhr = new XMLHttpRequest();
        xhr.open(method, src, sync);
        xhr.onreadystatechange = function () {
            if (xhr.readyState == 4) {
                if (xhr.status == 200) {
                    return;
                } else if (xhr.status) {
                }
            }
        };
        xhr.send(tag + "=" + data);
    } catch (e) {
        console.log(e);
    }
}

function httpsend_request(method,src,sync, callbfun,sendMsg = "")
{
    try {
        var xhr = new XMLHttpRequest();
        xhr.open(method, src, sync);
        xhr.onreadystatechange = function () {
            if (xhr.readyState == 4) {
                if (xhr.status == 200) {

                    var json = null;
                    try{
                        json = JSON.parse(xhr.responseText);
                    }catch (e) {
                    }
                    
                    if(json != null) {
                        if(json.code == "AUTH") {
                            // load("login");
                        }else{
                            callbfun(xhr.responseText);
                        }
                    }else{
                        callbfun(xhr.responseText);
                    }
                    xhr.abort();
                    xhr = null;
                    return;
                } else if (xhr.status) {
                    xhr.abort();
                    xhr = null;
                }
            }
        };
        xhr.send(sendMsg);
    } catch (e) {
        console.log(e);
    }
}




