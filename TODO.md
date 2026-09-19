
## ✅ Step 1: Исправление перезагрузки при нажатии на значок настроек
- [x] Модифицирован `loadScreen()` в `src/ui/ui_loaders.c` — используется `lv_async_call` для отложенной загрузки экрана, предотвращая перезагрузку при клике на кнопки боковой панели во время выполнения события.

## ✅ Step 2: Изменение "ПЛА" и "АБС" на "PLA" и "ABS"
- [x] `src/ui/components/ui_comp_homecomponent.c` строка 1093: `"PLA"`
- [x] `src/ui/components/ui_comp_homecomponent.c` строка 1119: `"ABS"`
- [x] `src/ui/components/ui_comp_homecomponent.c` строка 1145: `"OFF"` (было "Выкл")

## ✅ Step 3: Добавление надписи "Bambu Lab" в главном меню около статуса принтера
- [x] `src/ui/components/ui_comp_homecomponent.c` строки 980-989: Добавлен лейбл `cui_mainScreenBambuLabLabel` с зелёным цветом, шрифтом `lv_font_middle`, текстом `"Bambu Lab"` в компоненте `ui_mainStatusComponent`.

## ✅ Step 4: Очистка кода
- [x] `on_settings_screen_delete` — удалён
- [x] `s_cached_settings_sidebar` — удалён

## ✅ Step 5: Исправление перезагрузки при входе в экран температуры/вентиляторов
- [x] `src/ui/components/ui_comp_temperaturecomponent.c`: Изменены `LV_EVENT_ALL` → `LV_EVENT_CLICKED` для 17 обработчиков событий (5 панелей температуры/вентиляторов + 12 кнопок клавиатуры)
- [x] `src/ui/components/ui_comp_temperaturecomponent.c`: Добавлены проверки NULL для `ui_temperatureComponent` и `ui_temperatureComponent_input_target` в 6 функциях
- [x] `src/ui/ui_loaders.c`: Добавлен screen 1 в защиту `xtouch_settings_initializing` (аналогично screen 4)

## ✅ Step 6: Исправление перезагрузки при переключении вкладок (ОСИ/ГОЛОВКА и др.)
- [x] `src/ui/components/ui_comp_controlcomponent.c`: Изменены `LV_EVENT_ALL` → `LV_EVENT_CLICKED` для 8 обработчиков кнопок
- [x] `src/ui/components/ui_comp_homecomponent.c`: Изменены `LV_EVENT_ALL` → `LV_EVENT_CLICKED` для 7 кнопок и `LV_EVENT_ALL` → `LV_EVENT_VALUE_CHANGED` для 2 выпадающих списков
- [x] `src/ui/components/ui_comp_filamentcomponent.c`: Изменены `LV_EVENT_ALL` → `LV_EVENT_CLICKED` для 5 обработчиков кнопок

## ✅ Step 7: Полное удаление всех оставшихся LV_EVENT_ALL из компонентов
- [x] `src/ui/components/ui_comp_sidebarcomponent.c`: 6 кнопок навигации `LV_EVENT_ALL` → `LV_EVENT_CLICKED` (Главная, Принтеры, История, Температура, AMS, Настройки)
- [x] `src/ui/components/ui_comp_confirmpanel.c`: 2 кнопки `LV_EVENT_ALL` → `LV_EVENT_CLICKED` (НЕТ, ДА)
- [x] `src/ui/components/ui_comp_hmspanel.c`: 3 кнопки `LV_EVENT_ALL` → `LV_EVENT_CLICKED` (Подтвердить, Возобновить, Готово)
- [x] `src/ui/components/ui_comp_optionalcomponent.c`: 3 слайдера `LV_EVENT_ALL` → `LV_EVENT_VALUE_CHANGED | LV_EVENT_RELEASED` (NeoPixel количество, яркость, таймаут сирены)
- [x] `src/ui/components/ui_comp_utilcalibrationcomponent.c`: 1 кнопка `LV_EVENT_ALL` → `LV_EVENT_CLICKED`
- [x] `src/ui/components/ui_comp_charactercomponent.c`: 1 кнопка `LV_EVENT_ALL` → `LV_EVENT_CLICKED`
- [x] `src/ui/components/ui_comp_utilnozzlechagnecomponent.c`: 1 кнопка → `LV_EVENT_CLICKED`, 2 dropdown → `LV_EVENT_VALUE_CHANGED`
- [x] `src/ui/components/ui_comp_utilcomponent.c`: 2 кнопки `LV_EVENT_ALL` → `LV_EVENT_CLICKED`
- [x] `src/ui/components/ui_comp_lightingpanel.c`: 2 кнопки `LV_EVENT_ALL` → `LV_EVENT_CLICKED`

