from typing import Optional, List
from datetime import datetime, timezone
from sqlmodel import SQLModel, Field, Relationship
from typing import TYPE_CHECKING

from entities.tipus_registre import TipusRegistreEnum

if TYPE_CHECKING:
    from entities.dispositiu import Dispositiu
    from entities.horari import Horari
    from entities.justificacio import Justificacio
    from entities.usuari import Usuari

class Assistencia(SQLModel, table=True):
    __tablename__ = "ASSISTENCIA"

    id: Optional[int] = Field(default=None, primary_key=True)
    usuari_id: int = Field(foreign_key="USUARIS.id")
    dispositiu_id: int = Field(foreign_key="DISPOSITIUS.id")
    horari_id: int = Field(foreign_key="HORARIS.id")
    timestamp: datetime = Field(default_factory=lambda: datetime.now(timezone))
    tipus_registre: TipusRegistreEnum

    usuari: "Usuari" = Relationship(back_populates="assistencies")
    dispositiu: "Dispositiu" = Relationship(back_populates="assistencies")
    horari: "Horari" = Relationship(back_populates="assistencies")
    justificacions: List["Justificacio"] = Relationship(back_populates="assistencia")

from entities.dispositiu import Dispositiu
from entities.horari import Horari
from entities.justificacio import Justificacio
from entities.usuari import Usuari