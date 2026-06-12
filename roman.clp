;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Экспертная система определения жанра романа
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(deffunction ask-question (?question $?allowed-values)
    (printout t crlf ?question)
    (bind ?answer (read))
    (if (lexemep ?answer)
        then
        (bind ?answer (lowcase ?answer)))
    (while (not (member$ ?answer ?allowed-values)) do
        (printout t crlf "Введите yes/no: ")
        (bind ?answer (read))
        (if (lexemep ?answer)
            then
            (bind ?answer (lowcase ?answer))))
    ?answer
)

(deffunction yesno (?question)
    (bind ?response
        (ask-question ?question yes no y n))
    (if (or (eq ?response yes)
            (eq ?response y))
        then
        yes
        else
        no)
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ВЫВОД РЕЗУЛЬТАТА
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defrule print-result
    ?r <- (result ?genre)
    =>
    (printout t crlf)
    (printout t "===================================" crlf)
    (printout t "Жанр: " ?genre crlf)
    (printout t "===================================" crlf crlf)
    (retract ?r)
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; КОРНЕВОЙ ВОПРОС
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defrule root-question
    ?s <- (start)
    =>
    (retract ?s)
    (assert
        (philosophy
            (yesno
                "В этом жанре преобладает философско-идеологическая рефлексия? (yes/no): ")))
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ЛЕВАЯ ВЕТВЬ
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defrule philosophy-no
    (philosophy no)
    =>
    (assert
        (social-links
            (yesno
                "Находятся ли в центре социальные связи между людьми? (yes/no): ")))
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ВЕТКА 3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defrule social-links-no
    (social-links no)
    =>
    (assert
        (personality
            (yesno
                "Рефлексия направлена преимущественно на целостное познание личности человека? (yes/no): ")))
)

(defrule personality-no
    (personality no)
    =>
    (assert
        (tension
            (yesno
                "Является ли доминантным настроением напряжение? (yes/no): ")))
)

(defrule tension-no
    (tension no)
    =>
    (assert (result "готический роман")))

(defrule tension-yes
    (tension yes)
    =>
    (assert (result "роман испытания")))

(defrule personality-yes
    (personality yes)
    =>
    (assert
        (real-life
            (yesno
                "Книга посвящена жизни реального человека? (yes/no): ")))
)

(defrule real-life-yes
    (real-life yes)
    =>
    (assert (result "биографический роман")))

(defrule real-life-no
    (real-life no)
    =>
    (assert
        (objective-reality
            (yesno
                "Вымысел представляется как объективная реальность? (yes/no): ")))
)

(defrule objective-reality-no
    (objective-reality no)
    =>
    (assert (result "психологический роман")))

(defrule objective-reality-yes
    (objective-reality yes)
    =>
    (assert (result "магический роман")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ВЕТКА 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defrule social-links-yes
    (social-links yes)
    =>
    (assert
        (social-interaction
            (yesno
                "Находятся ли в центре внимания именно социальные взаимодействия, а не межличностные? (yes/no): ")))
)

(defrule social-interaction-yes
    (social-interaction yes)
    =>
    (assert (result "социально-бытовой роман")))

(defrule social-interaction-no
    (social-interaction no)
    =>
    (assert
        (love-theme
            (yesno
                "Любовь является ключевой темой сюжета? (yes/no): ")))
)

(defrule love-theme-no
    (love-theme no)
    =>
    (assert (result "семейная сага")))

(defrule love-theme-yes
    (love-theme yes)
    =>
    (assert (result "любовный роман")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ПРАВАЯ ВЕТВЬ
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defrule philosophy-yes
    (philosophy yes)
    =>
    (assert
        (inner-categories
            (yesno
                "Рефлексия направлена на внутренние категории? (yes/no): ")))
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ВЕТКА 5
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defrule inner-categories-no
    (inner-categories no)
    =>
    (assert
        (ideal-language
            (yesno
                "Идеал хранится в языке? (yes/no): ")))
)

(defrule ideal-language-yes
    (ideal-language yes)
    =>
    (assert
        (ideal-ridiculed
            (yesno
                "Идеал высмеивается? (yes/no): ")))
)

(defrule ideal-ridiculed-no
    (ideal-ridiculed no)
    =>
    (assert (result "филологический роман")))

(defrule ideal-ridiculed-yes
    (ideal-ridiculed yes)
    =>
    (assert (result "сатирический роман")))

(defrule ideal-language-no
    (ideal-language no)
    =>
    (assert
        (religion
            (yesno
                "Этот жанр связан с религией? (yes/no): ")))
)

(defrule religion-yes
    (religion yes)
    =>
    (assert (result "религиозно-нравственный роман")))

(defrule religion-no
    (religion no)
    =>
    (assert
        (ideology-conflict
            (yesno
                "Строится ли в этом жанре конфликт между идеологией героя и социальным порядком? (yes/no): ")))
)

(defrule ideology-conflict-no
    (ideology-conflict no)
    =>
    (assert (result "политический роман")))

(defrule ideology-conflict-yes
    (ideology-conflict yes)
    =>
    (assert (result "социальный роман")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ВЕТКА 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defrule inner-categories-yes
    (inner-categories yes)
    =>
    (assert
        (meaning-question
            (yesno
                "Роман заставляет усомниться в том, есть ли в происходящем хоть какой-то смысл? (yes/no): ")))
)

(defrule meaning-question-no
    (meaning-question no)
    =>
    (assert
        (moral-search
            (yesno
                "Является ли целью поиск ответов на вопросы морали? (yes/no): ")))
)

(defrule moral-search-no
    (moral-search no)
    =>
    (assert (result "роман самосовершенствования")))

(defrule moral-search-yes
    (moral-search yes)
    =>
    (assert (result "философский роман")))

(defrule meaning-question-yes
    (meaning-question yes)
    =>
    (assert
        (strict-plot
            (yesno
                "Сюжет простой по структуре? (yes/no): ")))
)

(defrule strict-plot-no
    (strict-plot no)
    =>
    (assert (result "экспериментальный роман")))

(defrule strict-plot-yes
    (strict-plot yes)
    =>
    (assert (result "экзистенциальный роман")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ЗАПУСК СИСТЕМЫ
;;
;; (load "C:/Users/arllr/Downloads/roman.clp")
;; (reset)
;; (assert (start))
;; (run)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;