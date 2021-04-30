
// recuperation des elements

let couleur = document.getElementById('couleur');
let intensite = document.getElementById('intensite');
let effet = document.getElementById('effet');
const onBtn = document.getElementById('on');
const offBtn = document.getElementById('off');


// a l ecoute de changements

//---------------------------------------------  On
onBtn.addEventListener('click', (e)=>{
    e.preventDefault();
    console.log('click on');
    fetch('/ledOn').then(resp => resp.text()).then(resp => {
        if (resp == 'on'){
            onBtn.style.backgroundColor = 'green';
            offBtn.style.backgroundColor = 'initial';
        }
        else{
            onBtn.style.backgroundColor = 'initial';
            offBtn.style.backgroundColor = 'red';
        }
    }).catch(err => console.log(err));
});

//-----------------------------------------------  off
offBtn.addEventListener('click', (e)=>{
    e.preventDefault();
    console.log('click off');
    fetch('/ledOff').then(resp => resp.text()).then(resp => {
        if (resp == 'off'){
            onBtn.style.backgroundColor = 'initial';
            offBtn.style.backgroundColor = 'red';
        }
        else{
            onBtn.style.backgroundColor = 'green';
            offBtn.style.backgroundColor = 'initial';
        }
    }).catch(err => console.log(err));
});


//----------------------------------------------- COULEUR
couleur.addEventListener('change', function(e){
    console.log(e.target.value);

    const val = e.target.value.substring(1, e.target.value.length);
    const addresse = '/LED?couleur=%23' + val;
    fetch(addresse).then(resp => console.log('ok')).catch(err => console.log(err));

});

//------------------------------------------------- INTENSITE
intensite.addEventListener('change', function(e){
    console.log(e.target.value);

    fetch('/LED?intensite=' + e.target.value)
    .then(resp => console.log('ok'))
    .catch(err => console.log(err));
});


//-------------------------------------------- EFFETS ------------
effet.addEventListener('change', function(e){
    console.log(e.target.value);

    if(e.target.value != 'Aucun'){
        couleur.disabled = true;

        const box = document.getElementById('box_coul').style.backgroundColor = 'grey';
    }
    else{
        couleur.disabled = false;
        const box = document.getElementById('box_coul').style.backgroundColor = '#eeeeee';
    }


    fetch('/LED?effet=' + e.target.value)
    .then(resp => console.log('ok'))
    .catch(err => console.log(err));
});