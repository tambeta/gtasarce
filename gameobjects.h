#ifndef _GAMEOBJECTS_H
#define _GAMEOBJECTS_H 1

typedef struct {
    gchar *name;
    gchar *group;
    int id;
} GameObject;

GPtrArray *go_match_name(const GameObject[], gchar*);
GPtrArray *go_match_id(const GameObject go[],  int id);

static const GameObject GO_RACES[] = {
    { "Lowrider Race",          "", 0 },
    { "Little Loop",            "", 1 },
    { "Backroad Wanderer",      "", 2 },
    { "City Circuit",           "", 3 },
    { "Vinewood",               "", 4 },
    { "Freeway",                "", 5 },
    { "Into the Country",       "", 6 },    
    { "Badlands A",             "", 7 },
    { "Badlands B",             "", 8 },
    
    { "Dirtbike Danger",        "", 9 },
    { "Bandito Country",        "", 10 },
    { "Go-Go Karting",          "", 11 },
    { "SF Fastlane",    "", 12 },
    { "SF Hills",       "", 13 },
    { "Country Endurance",      "", 14 },
    
    { "SF to LV",               "", 15 },    
    { "Dam Rider",              "", 16 },
    { "Desert Tricks",          "", 17 },    
    { "LV Ringroad",  "", 18 },
    { NULL }
};

static const GameObject GO_VEHICLES[] = {
    { "Landstalker", "", 400},
    { "Bravura", "", 401 },
    { "Buffalo", "", 402 },
    { "Linerunner", "", 403 },
    { "Perennial", "", 404 },
    { "Sentinel", "", 405 },
    { "Dumper", "", 406 },
    { "Firetruck A", "", 407 },
    { "Trashmaster", "", 408 },
    { "Stretch", "", 409 },
    { "Manana", "", 410 },
    { "Infernus", "", 411 },
    { "Voodoo", "", 412 },
    { "Pony", "", 413 },
    { "Mule", "", 414 },
    { "Cheetah", "", 415 },
    { "Ambulance", "", 416 },
    { "Moonbeam", "", 418 },
    { "Esperanto", "", 419 },
    { "Taxi", "", 420 },
    { "Washington", "", 421 },
    { "Bobcat", "", 422 },
    { "Mr. Whoopee", "", 423 },
    { "BF Injection", "", 424 },
    { "Premier", "", 426 },
    { "Enforcer", "", 427 },
    { "Securicar", "", 428 },
    { "Banshee", "", 429 },
    { "Bus", "", 431 },
    { "Rhino", "", 432 },
    { "Barracks OL", "", 433 },
    { "Hotknife", "", 434 },
    { "Previon", "", 436 },
    { "Coach", "", 437 },
    { "Cabbie", "", 438 },
    { "Stallion", "", 439 },
    { "Rumpo", "", 440 },
    { "RC Bandit", "", 441 },
    { "Romero", "", 442 },
    { "Packer", "", 443 },
    { "Monster A", "", 444 },
    { "Admiral", "", 445 },
    { "Pizza Boy", "", 448 },
    { "Turismo", "", 451 },
    { "Flatbed", "", 455 },
    { "Yankee", "", 456 },
    { "Caddy", "", 457 },
    { "Solair", "", 458 },
    { "Top Fun", "", 459 },
    { "PCJ-600", "", 461 },
    { "Faggio", "", 462 },
    { "Freeway", "", 463 },
    { "Glendale", "", 466 },
    { "Oceanic", "", 467 },
    { "Sanchez", "", 468 },
    { "Patriot", "", 470 },
    { "Hermes", "", 474 },
    { "Sabre", "", 475 },
    { "ZR-350", "", 477 },
    { "Walton", "", 478 },
    { "Regina", "", 479 },
    { "Comet", "", 480 },
    { "Burrito", "", 482 },
    { "Camper", "", 483 },
    { "Baggage Handler", "", 485 },
    { "Dozer", "", 486 },
    { "Rancher A", "", 489 },
    { "FBI Rancher", "", 490 },
    { "Virgo", "", 491 },
    { "Greenwood", "", 492 },
    { "Hotring A", "", 494 },
    { "Sandking", "", 495 },
    { "Blista Compact", "", 496 },
    { "Boxville", "", 498 },
    { "Benson", "", 499 },
    { "Mesa", "", 500 },
    { "Hotring B", "", 502 },
    { "Hotring C", "", 503 },
    { "Bloodring Banger", "", 504 },
    { "Rancher B", "", 505 },
    { "Super GT", "", 506 },
    { "Elegant", "", 507 },
    { "Journey", "", 508 },
    { "Tanker", "", 514 },
    { "Roadtrain", "", 515 },
    { "Nebula", "", 516 },
    { "Majestic", "", 517 },
    { "Buccaneer", "", 518 },
    { "FCR-900", "", 521 },
    { "NRG-500", "", 522 },
    { "HPV-1000", "", 523 },
    { "Cement", "", 524 },
    { "Tow Truck", "", 525 },
    { "Fortune", "", 526 },
    { "Cadrona", "", 527 },
    { "FBI Truck", "", 528 },
    { "Willard", "", 529 },
    { "Forklift", "", 530 },
    { "Tractor", "", 531 },
    { "Combine", "", 532 },
    { "Feltzer", "", 533 },
    { "Remington", "", 534 },
    { "Slamvan", "", 535 },
    { "Blade", "", 536 },
    { "Vincent", "", 540 },
    { "Bullet", "", 541 },
    { "Clover", "", 542 },
    { "Sadler", "", 543 },
    { "Firetruck B", "", 544 },
    { "Hustler", "", 545 },
    { "Intruder", "", 546 },
    { "Primo", "", 547 },
    { "Tampa", "", 549 },
    { "Sunrise", "", 550 },
    { "Merit", "", 551 },
    { "Utility Van", "", 552 },
    { "Yosemite", "", 554 },
    { "Windsor", "", 555 },
    { "Monster B", "", 556 },
    { "Monster C", "", 557 },
    { "Uranus", "", 558 },
    { "Jester", "", 559 },
    { "Sultan", "", 560 },
    { "Stratum", "", 561 },
    { "Elegy", "", 562 },
    { "RC Tiger", "", 564 },
    { "Flash", "", 565 },
    { "Tahoma", "", 566 },
    { "Savanna", "", 567 },
    { "Bandito", "", 568 },
    { "Kart", "", 571 },
    { "Mover", "", 572 },
    { "Dune", "", 573 },
    { "Sweeper", "", 574 },
    { "Broadway", "", 575 },
    { "Tornado", "", 576 },
    { "DFT-30", "", 578 },
    { "Huntley", "", 579 },
    { "Stafford", "", 580 },
    { "BF-400", "", 581 },
    { "News Van", "", 582 },
    { "Tug", "", 583 },
    { "Emperor", "", 585 },
    { "Wayfarer", "", 586 },
    { "Euros", "", 587 },
    { "Hotdog", "", 588 },
    { "Club", "", 589 },
    { "RC Cam", "", 594 },
    { "Police LS", "", 596 },
    { "Police SF", "", 597 },
    { "Police LV", "", 598 },
    { "Police Ranger", "", 599 },
    { "Picador", "", 600 },
    { "Swat Van", "", 601 },
    { "Alpha", "", 602 },
    { "Phoenix", "", 603 },
    { "Damaged Glendale", "", 604 },
    { "Damaged Sadler", "", 605 },
    { "Boxville", "", 609 },    
    { NULL },
};

#endif