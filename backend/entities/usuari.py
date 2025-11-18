from typing import Optional, List, Literal, TYPE_CHECKING
from sqlmodel import SQLModel, Field, Relationship

from entities.tipus_usuari import TipusUsuariEnum

if TYPE_CHECKING:
    from entities.alerta import Alerta
    from entities.assistencia import Assistencia
    from entities.usuari_classe import UsuariClasse

class Usuari(SQLModel, table=True):
    __tablename__ = "USUARIS"

    id: Optional[int] = Field(default=None, primary_key=True)
    nom: str
    cognoms: str
    tipus: TipusUsuariEnum
    targeta_rfid: Optional[str] = Field(default=None, unique=True)
    actiu: bool = Field(default=True)

    assistencies: List["Assistencia"] = Relationship(back_populates="usuari")
    alertes: List["Alerta"] = Relationship(back_populates="usuari")
    usuaris_classes: List["UsuariClasse"] = Relationship(back_populates="usuari")

from entities.assistencia import Assistencia
from entities.alerta import Alerta
from entities.usuari_classe import UsuariClasse