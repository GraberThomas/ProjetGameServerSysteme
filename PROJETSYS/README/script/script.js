var hum = document.getElementById("hamburger");
var submenu = document.getElementById("submenu");
var nav = document.getElementById("nav")

var bol = false;


hum.addEventListener('click', event =>{
    if(bol === false){
        hum.style.transform = 'rotate(90deg)';
        console.log("yes");
        submenu.style.display = 'block';
        bol = true;
    }else{
        hum.style.transform = 'rotate(0deg)';
        console.log("no");
        submenu.style.display = 'none';
        bol = false;
    }
});

