document.addEventListener('DOMContentLoaded', () => {
  const btn = document.getElementById("gameButton");
  const scoreElement = document.getElementById("score");
  const bagImage = document.querySelector('.bag-image');

  if (parseInt(localStorage.getItem("score")) !== 0) {
    scoreElement.textContent = parseInt(scoreElement.textContent) + parseInt(localStorage.getItem("score"));
    updateDB();
  }

  const giftNames = [
    '/static/img/gifts/gift1.png',
    '/static/img/gifts/gift2.png',
    '/static/img/gifts/gift3.png',
    '/static/img/gifts/gift4.png',
    '/static/img/gifts/gift5.png',
    '/static/img/gifts/gift6.png',
    '/static/img/gifts/gift7.png'
];

let currentGiftIndex = 0;
let directionToggle = true;

btn.addEventListener('click', () => {
    bagImage.style.transform = 'scale(0.95)';
    scoreElement.textContent = parseInt(scoreElement.textContent) + 1;
    localStorage.setItem("score", parseInt(localStorage.getItem("score")) + 1); 

    createGift();

    setTimeout(() => {
        bagImage.style.transform = 'scale(1)';
    }, 100); 

    currentGiftIndex = (currentGiftIndex + 1) % 7; 
});

const keyframes = `
  @keyframes fly {
    0% {
      transform: translate(0, 0);
      opacity: 1;
    }
    40% {
      transform: translate(var(--halfDiagonalX), var(--arcHeight));
      opacity: 1;
    }
    80% {
      transform: translate(var(--diagonalX), var(--arcHeight * 0.4));
      opacity: 1;
    }
    100% {
      transform: translate(var(--diagonalX), var(--diagonalY));
      opacity: 0;
    }
  }
`;

const style = document.createElement('style');
style.innerHTML = keyframes;
document.head.appendChild(style);

function createGift() {
    const gift = document.createElement('div');
    gift.className = 'gift';
    gift.style.backgroundImage = `url('/static/img/gifts/gift${currentGiftIndex + 1}.png')`;

    const bagRect = bagImage.getBoundingClientRect();
    const giftWidth = 80;
    const halfGiftWidth = giftWidth / 2;

    gift.style.left = `${bagRect.left + (bagRect.width / 2) - (halfGiftWidth)}px`;
    gift.style.top = `${bagRect.top}px`;

    const randomDirection = directionToggle ? 1 : -1;
    const diagonalX = randomDirection * 200; 
    const diagonalY = Math.random() * 100 + 50;
    const halfDiagonalX = diagonalX / 2;
    const arcHeight = -Math.abs(diagonalY * 0.3);

    gift.style.setProperty('--diagonalX', `${diagonalX}px`);
    gift.style.setProperty('--diagonalY', `${diagonalY}px`);
    gift.style.setProperty('--halfDiagonalX', `${halfDiagonalX}px`);
    gift.style.setProperty('--arcHeight', `${arcHeight}px`);

    gift.style.zIndex = 1000;
    gift.style.position = 'absolute';
    document.body.appendChild(gift);

    gift.style.animation = 'fly 2s ease-out forwards'; 


    setTimeout(() => {
        gift.remove();
    }, 3500); 

    directionToggle = !directionToggle; 
}


  function updateDB() {
    let cookie = "" + document.cookie;
    fetch('/update_score', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Cookie': `${cookie}`
      },
      body: JSON.stringify({ score: parseInt(localStorage.getItem("score")) })
    })
    .then(response => {
      if (response.ok) {
        localStorage.setItem("score", 0);
      }
    });
  }

  window.addEventListener('beforeunload', function(event) {
    updateDB();
  });

  setInterval(updateDB, 5000); 
});
