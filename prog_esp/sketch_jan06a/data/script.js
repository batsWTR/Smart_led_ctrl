
// recuperation des elements

let couleur = document.getElementById('couleur');
let intensite = document.getElementById('intensite');
let effet = document.getElementById('effet');



// a l ecoute de changements

couleur.addEventListener('change', function(e){
    console.log(e.target.value);
    let demande = new XMLHttpRequest();
    demande.open('POST', '/LED');
    demande.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    demande.send('couleur=' + e.target.value);


});

intensite.addEventListener('change', function(e){
    console.log(e.target.value);
    let demande = new XMLHttpRequest();
    demande.open('POST', '/LED');
    demande.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    demande.send('intensite=' + e.target.value);
});

effet.addEventListener('change', function(e){
    console.log(e.target.value);
    let demande = new XMLHttpRequest();
    demande.open('POST', '/LED');
    demande.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    demande.send('effet=' + e.target.value);
});